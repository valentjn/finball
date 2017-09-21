#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>
#include <string>
#include <fstream>
#include <stdexcept>
#include <iostream>

#include <renderer/CompRenderer.hpp>

// Compiles a shader
GLuint createShader(const char* file_path, GLuint shader_type)
{
	std::ifstream file_stream{ file_path };
	if (!file_stream)
		throw std::runtime_error(std::string{ "Can't find " } + file_path);
	std::string file_content{ std::istreambuf_iterator<char>(file_stream), std::istreambuf_iterator<char>() };
	const GLchar* file_data = file_content.c_str();
	GLuint shader = glCreateShader(shader_type);
	glShaderSource(shader, 1, &file_data, nullptr);
	glCompileShader(shader);
	GLint success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		std::string msg{ "Failed to compile shader at " };
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
GLuint createProgram(const char* vert_path, const char* frag_path)
{
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

CompRenderer::CompRenderer()
	: m_resolution(960, 540)
	, m_camera_pos(0.f, 0.f, 5.f)

{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		throw std::runtime_error(SDL_GetError());

	// create window
	m_window = SDL_CreateWindow(
		"Game",											// title
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,	// position
		m_resolution.x, m_resolution.y,					// size
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!m_window)
		throw std::runtime_error(SDL_GetError());

    // initialize OpenGL
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GLContext m_glcontext = SDL_GL_CreateContext(m_window);
	if (!m_glcontext)
		throw std::runtime_error(SDL_GetError());
	std::cout << "OpenGL " << glGetString(GL_VERSION) << '\n';
	std::cout << "GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

    // create shader programs
	m_shader_program_world = createProgram(
		"src/renderer/world_vert.glsl",
		"src/renderer/world_frag.glsl");
    m_shader_program_ui = createProgram( // TODO: use different shaders
        "src/renderer/world_vert.glsl",
        "src/renderer/world_frag.glsl");

	// full window viewport
	glViewport(0, 0, m_resolution.x, m_resolution.y);

    // create a rectangle mesh so that we have some dummy to render (WIP)
	m_rectangle = std::unique_ptr<Mesh>(new Mesh());

	// create the texture for the velocities of the fluid
	glGenTextures(1, &m_fluid_tex);
}

CompRenderer::~CompRenderer()
{
	SDL_GL_DeleteContext(m_glcontext);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

void CompRenderer::setFluidVecs(const Array2D<glm::vec3>& fluid_vecs)
{
	// upload texture
	glBindTexture(GL_TEXTURE2D, m_tex_fluid);
	glTexImage2D(GL_TEXTURE2D,
		0,			// mipmap level
		GL_RGB,		// internal format
		100,		// width
		100,		// height
		0,			// must be 0, according to khronos.org
		GL_RGB,		// data format
		GL_FLOAT,	// data format
		fluid_vecs.getData()); // data pointer
	glBindTexture(GL_TEXTURE2D, 0);

	
	
	// upload width and height of the texture
	/*glUseShaderProgram(shader_program_fluid);
	glUniform1i(glGetUniformLocation(m_shader_program_fluid, "vecs_width"), fluid_vecs.width());
	glUniform1i(glGetUniformLocation(m_shader_program_fluid, "vecs_height"), fluid_vecs.height());
	glUseShaderProgram(0);*/
}

// enqueues a world object for rendering (at the next update)
void CompRenderer::renderWorldObject(const RenderObject& obj)
{
	m_world_objects.push_back(obj);
}

// enqueues a ui object for rendering (at the next update)
void CompRenderer::renderUIObject(const RenderObject& obj)
{
	m_ui_objects.push_back(obj);
}

bool CompRenderer::update(typename CompRenderer::OutputData&)
{
	updateFluidTex();

    // clear the framebuffer to dark red
	glClearColor(0.4f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// render fluid
	glUseProgram(m_shader_program_fluid);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE2D, m_tex_fluid;
	m_rectangle->render();

    // setup for rendering the world objects
	glUseProgram(m_shader_program_world);

    // update view matrix in shader_program_world
	glm::mat4 view = glm::lookAt(
		m_camera_pos,							// eye
		m_camera_pos + glm::vec3{ 0, 0, -1 },	// center
		glm::vec3{ 0, 1, 0 });					// up
	glUniformMatrix4fv(
		glGetUniformLocation(m_shader_program_world, "view"),
		1,										// matrix count
		GL_FALSE,								// is not transposed
		glm::value_ptr(view));					// data pointer

    // update projection matrix in shader_program_world
	glm::mat4 projection = glm::perspective(
		glm::pi<float>() * 0.25f,								// vertical field of view
		static_cast<float>(m_resolution.x) / m_resolution.y,	// aspect ratio
		0.1f, 10.f);											// distance near & far plane
	glUniformMatrix4fv(
		glGetUniformLocation(m_shader_program_world, "projection"),
		1,										// matrix count
		GL_FALSE,								// is not transposed
		glm::value_ptr(projection));			// data pointer

	// render the world objects
	GLint model_location = glGetUniformLocation(m_shader_program_world, "model");
	for (const RenderObject& object : m_world_objects)
        render(object, model_location);
	m_world_objects.clear();

    // render the ui object
	glUseProgram(m_shader_program_ui);
	model_location = glGetUniformLocation(m_shader_program_ui, "model");
	for (const RenderObject& object : m_ui_objects)
        render(object, model_location);
	m_ui_objects.clear();

	// Swap back and front buffer
	SDL_GL_SwapWindow(m_window);

    return true;
}

// renders an object to the screen (private method)
void CompRenderer::render(const RenderObject& object, GLint model_location) const
{
    glm::mat4 model;
	model = glm::translate(model, object.position);
    //model = glm::scale(model, glm::vec3{ object.scale });
	glUniformMatrix4fv(
		model_location,         // uniform location of the model matrix in the shader
		1,                      // matrix count
		GL_FALSE,               // is not transposed
		glm::value_ptr(model)); // data pointer
	m_rectangle->render();
}

