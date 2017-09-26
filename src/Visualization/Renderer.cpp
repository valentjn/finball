#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <chrono>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Log.hpp"
#include "LevelDesign/Level.hpp"
#include "SDL/SDLWindow.hpp"
#include "Visualization/Renderer.hpp"
#include "Visualization/SimplexNoise.hpp"

// Compiles a shader
GLuint createShader(const char *file_path, GLuint shader_type) {
    std::ifstream file_stream{file_path};
    if (!file_stream)
        throw std::runtime_error(std::string{"Can't find "} + file_path);
    std::string file_content{std::istreambuf_iterator<char>(file_stream),
                             std::istreambuf_iterator<char>()};
    const GLchar *file_data = file_content.c_str();
    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &file_data, nullptr);
    glCompileShader(shader);
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        std::string msg{"Failed to compile shader at "};
        msg += file_path;
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        if (length != 0) {
            msg += '\n';
            std::vector<GLchar> log(length);
            glGetShaderInfoLog(shader, length, nullptr, log.data());
            msg += log.data();
        }
        throw std::runtime_error(msg);
    }
    return shader;
}

// Links a shader program consisting of a vertex and a fragment shader
GLuint createProgram(const char *vert_path, const char *frag_path) {
    GLuint shader_program = glCreateProgram();
    GLuint vert_shader = createShader(vert_path, GL_VERTEX_SHADER);
    GLuint frag_shader = createShader(frag_path, GL_FRAGMENT_SHADER);
    glAttachShader(shader_program, vert_shader);
    glAttachShader(shader_program, frag_shader);
    glLinkProgram(shader_program);
    glDetachShader(shader_program, vert_shader);
    glDetachShader(shader_program, frag_shader);
    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);
    GLint success = 0;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success)
        throw std::runtime_error("Failed to link shader program.");
    return shader_program;
}

void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                            const GLchar *message, const void *userParam) {
    Log::debug(message);
}

// TODO: dynamically set camera positon depending on level size
Renderer::Renderer(const SDLWindow &window) : m_camera_pos(32.f, -16.f, 64.f) {
    m_window = window.getWindow();
    m_resolution = glm::ivec2(window.getWidth(), window.getHeight());

    // initialize OpenGL
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    m_glcontext = SDL_GL_CreateContext(m_window);
    if (!m_glcontext) {
        auto error = SDL_GetError();
        Log::error("Failed to create SDL window: %s", error);
        throw std::runtime_error(error);
    }

    Log::debug("Loaded Renderer with OpenGL version:");
    Log::debug("OpenGL %s", glGetString(GL_VERSION));
    Log::debug("GLSL %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    //glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    //glDebugMessageCallback(&debugCallback, nullptr);
    //GLuint unusedIDs = 0;
    //glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIDs, GL_TRUE);

    int glError = glGetError();
    if (glError != 0)
        Log::error("OpenGL Error: %d", glError);

    // create shader programs
    m_shader_program_world = createProgram(
        "src/Visualization/glsl/world_vert.glsl",
        "src/Visualization/glsl/world_frag.glsl");
    m_shader_program_ui = createProgram( // TODO: use different shaders
        "src/Visualization/glsl/ui_vert.glsl",
        "src/Visualization/glsl/world_frag.glsl");
    m_shader_program_fluid = createProgram(
        "src/Visualization/glsl/fluid_vert.glsl",
        "src/Visualization/glsl/fluid_frag.glsl");

    // create the noise texture
    Array2D<glm::vec2> noise(m_fluid_width / 2, m_fluid_height / 2);
    std::default_random_engine engine;
    std::uniform_real_distribution<float> dist{ 0.0f, 1.0f };
	constexpr float noise_scale = 0.03f;
    for (int i = 0; i < noise.width(); ++i)
        for(int j = 0; j < noise.height(); ++j)
			noise.value(i, j) =
				glm::vec2{ dist(engine), simplex::noise2D(i * noise_scale, j * noise_scale) * 0.5f + 0.5f };
    m_tex_noise = std::make_unique<Texture2F>(glm::ivec2{ noise.width(), noise.height() });
    m_tex_noise->setData(noise);
	m_tex_noise->bind(0);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// create the waves texture
	m_tex_waves1 = std::make_unique<Texture1F>(glm::ivec2{ m_fluid_width, m_fluid_height });
	m_tex_waves2 = std::make_unique<Texture1F>(glm::ivec2{ m_fluid_width, m_fluid_height });
	m_tex_waves1->bind(0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	m_tex_waves2->bind(0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // create lic output texture
    m_tex_fluid_output = std::make_unique<Texture3F>(glm::ivec2{ m_fluid_width, m_fluid_height });

	// create intermediate framebuffers
    glGenFramebuffers(static_cast<GLsizei>(m_framebuffers_fluid_output.size()), m_framebuffers_fluid_output.data());
	for (size_t i = 0; i < m_framebuffers_fluid_output.size(); ++i) {
		glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffers_fluid_output[i]);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_tex_fluid_output->texture(), 0);
		if (i == 0)
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_tex_waves1->texture(), 0);
		else
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_tex_waves2->texture(), 0);
		std::array<GLenum, 2> draw_buffers{ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(static_cast<GLsizei>(draw_buffers.size()), draw_buffers.data());
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			Log::error("Failed to create framebuffer for fluid visualization.");
	}
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	m_inter_fb1 = true;

    // create a rectangle mesh to use in the first render pass where the fluid is visualized
    auto full_quad = Mesh::createRectangle(glm::vec2{-1, -1}, glm::vec2{1, 1});
	m_full_quad = std::make_unique<ColoredMesh>(full_quad, glm::vec3{});

    glEnable(GL_DEPTH_TEST);
}

Renderer::~Renderer() {
    SDL_GL_DeleteContext(m_glcontext);
}

void Renderer::update(const RendererInput &input) {
    // clear the framebuffer to black
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // setup for the rendering of the fluid
    glViewport(0, 0, m_fluid_width, m_fluid_height);
    glUseProgram(m_shader_program_fluid);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffers_fluid_output[m_inter_fb1 ? 1 : 0]);

    // bind & fill fluid input texture
    auto loc = glGetUniformLocation(m_shader_program_fluid, "tex_vecs");
    glUniform1i(loc, 0);
    if (!m_tex_fluid_input)
        m_tex_fluid_input = std::make_unique<Texture3F>(glm::ivec2{ input.fluid_input.width(), input.fluid_input.height() });
    m_tex_fluid_input->bind(0);
    m_tex_fluid_input->setData(input.fluid_input);

    // bind noise texture
    loc = glGetUniformLocation(m_shader_program_fluid, "tex_noise");
    glUniform1i(loc, 1);
    m_tex_noise->bind(1);

	// bind waves texture
	loc = glGetUniformLocation(m_shader_program_fluid, "tex_waves");
	glUniform1i(loc, 2);
	if (m_inter_fb1)
		m_tex_waves1->bind(2);
	else
		m_tex_waves2->bind(2);

	// set time
	static unsigned int ticks = 0;
	loc = glGetUniformLocation(m_shader_program_fluid, "t");
	++ticks;
	glUniform1ui(loc, static_cast<float>(ticks));

    // render fluid
	m_full_quad->render(0);

    // setup for rendering the world objects
    glViewport(0, 0, m_resolution.x, m_resolution.y);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(m_shader_program_world);
    input.fluid_mesh->setTexture(*m_tex_fluid_output);

    // update view matrix in shader_program_world
    glm::mat4 view = glm::lookAt(m_camera_pos,                       // eye
                                 glm::vec3{32, 24, 0}, // center
                                 glm::vec3{0, 1, 0});                // up
    glUniformMatrix4fv(glGetUniformLocation(m_shader_program_world, "view"),
                       1,                     // matrix count
                       GL_FALSE,              // is not transposed
                       glm::value_ptr(view)); // data pointer

    // update projection matrix in shader_program_world
    glm::mat4 projection = glm::perspective(
        glm::pi<float>() * 0.25f,                               // vertical field of view
        static_cast<float>(m_resolution.x) / m_resolution.y,    // aspect ratio
        1.0f, 256.f);                                           // distance near & far plane
    glUniformMatrix4fv(
        glGetUniformLocation(m_shader_program_world, "projection"),
        1,                           // matrix count
        GL_FALSE,                    // is not transposed
        glm::value_ptr(projection)); // data pointer

    // render the world objects
    GLint model_location = glGetUniformLocation(m_shader_program_world, "model");
    GLint mode_location = glGetUniformLocation(m_shader_program_world, "mode");
    for (const RenderObject &object : input.world_objects)
        render(object, model_location, mode_location);

    // render the ui object
    glUseProgram(m_shader_program_ui);
    model_location = glGetUniformLocation(m_shader_program_ui, "model");
    mode_location = glGetUniformLocation(m_shader_program_world, "mode");
    for (const RenderObject &object : input.ui_objects)
        render(object, model_location, mode_location);

    // Swap back and front buffer
    SDL_GL_SwapWindow(m_window);

	m_inter_fb1 = !m_inter_fb1;
}

// renders an object to the screen (private method)
void Renderer::render(const RenderObject &object, GLint model_location, GLint mode_location) const {
    glm::mat4 model;
	model = glm::translate(model, object.position);
    model = glm::scale(model, glm::vec3{ object.scale, 1 });
    model = glm::rotate(model, object.rotation, glm::vec3{ 0, 0, 1 });
	glUniformMatrix4fv(
		model_location,         // uniform location of the model matrix in the shader
		1,                      // matrix count
		GL_FALSE,               // is not transposed
		glm::value_ptr(model)); // data pointer
	assert(object.mesh);
	object.mesh->render(mode_location);
}
