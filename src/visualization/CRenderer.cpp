#include <visualization/CRenderer.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>
#include <string>
#include <fstream>
#include <stdexcept>


#include <iostream>

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

GLuint createProgram(const char* vert_path, const char* frag_path)
{
	GLuint shader_program = glCreateProgram();
	GLuint vert_shader = createShader(vert_path, GL_VERTEX_SHADER);
	GLuint frag_shader = createShader(frag_path, GL_FRAGMENT_SHADER);
	glAttachShader(shader_program, vert_shader);
	glAttachShader(shader_program, frag_shader);
	glLinkProgram(shader_program);
	//glDetachShader(shader_program, vert_shader);
	//glDetachShader(shader_program, frag_shader);
	//glDeleteProgram(vert_shader);
	//glDeleteProgram(frag_shader);
	GLint success = 0;
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if (!success)
		throw std::runtime_error("Failed to link shader program.");
	return shader_program;
}

CRenderer::CRenderer()
	: m_resolution(960, 540)
	, m_camera_pos(0.f, 0.f, 5.f)
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		throw std::runtime_error(SDL_GetError());

	// create m_window
	m_window = SDL_CreateWindow(
		"Game",											// title
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,	// position
		m_resolution.x, m_resolution.y,					// size
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!m_window)
		throw std::runtime_error(SDL_GetError());

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GLContext m_glcontext = SDL_GL_CreateContext(m_window);
	if (!m_glcontext)
		throw std::runtime_error(SDL_GetError());

	std::cout << "OpenGL " << glGetString(GL_VERSION) << '\n';
	std::cout << "GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

	m_shader_program_world = createProgram(
		"src/visualization/world_vert.glsl",
		"src/visualization/world_frag.glsl");
	//glBindAttribLocation(m_shader_program_world, 0, "in_pos");

	// full window viewport
	glViewport(0, 0, m_resolution.x, m_resolution.y);

	
}

CRenderer::~CRenderer()
{
	SDL_GL_DeleteContext(m_glcontext);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

void CRenderer::renderWorldObject(const CRenderObject& obj)
{
	m_world_objects.push_back(obj);
}

void CRenderer::renderUIObject(const CRenderObject& obj)
{
	m_ui_objects.push_back(obj);
}

void CRenderer::present()
{
	glClearColor(0.4f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set up rendering of the world objects
	glUseProgram(m_shader_program_world);
	glm::mat4 view = glm::lookAt(
		m_camera_pos,							// eye
		m_camera_pos + glm::vec3{ 0, 0, -1 },	// center
		glm::vec3{ 0, 1, 0 });					// up
	glm::mat4 projection = glm::perspective(
		glm::pi<float>() * 0.25f,								// vertical field of view
		static_cast<float>(m_resolution.x) / m_resolution.y,	// aspect ratio
		0.1f, 10.f);											// distance near & far plane
	glUniformMatrix4fv(
		glGetUniformLocation(m_shader_program_world, "view"),
		1,										// matrix count
		GL_FALSE,								// is not transposed
		glm::value_ptr(view));					// data pointer
	glUniformMatrix4fv(
		glGetUniformLocation(m_shader_program_world, "projection"),
		1,										// matrix count
		GL_FALSE,								// is not transposed
		glm::value_ptr(projection));			// data pointer
	//auto model_location = glGetUniformLocation(m_shader_program_world, "model");

	// actually render the world objects
	for (const CRenderObject& obj : m_world_objects) {
		glm::mat4 model = glm::translate(glm::mat4{}, obj.position);
		glUniformMatrix4fv(
			glGetUniformLocation(m_shader_program_world, "model"),
			1,
			GL_FALSE,
			glm::value_ptr(model));
		m_rectangle.render();
	}

	// clear the list of world objects, the list has to be newly filled for the next frame
	m_world_objects.clear();

	// set up rendering of the ui objects
	// glUseProgram(m_shader_program_ui);

	// actually render the ui objects
	for (const CRenderObject& obj : m_ui_objects) {
		// render 
	}

	// clear the list of ui objects, the list has to be newly filled for the next frame
	m_ui_objects.clear();

	// Swap back and front buffer
	SDL_GL_SwapWindow(m_window);

	std::cout << "Attrib: " << glGetAttribLocation(m_shader_program_world, "in_pos") << '\n';
}
