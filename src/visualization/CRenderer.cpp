#include <CRenderer.hpp>
#include <sstream>
#include <string>
#include <fstream>

GLuint createShader(const char* file_path)
{
	std::ifstream file_stream{ file_path };
	std::istringstream iss{ std::istreambuf_iterator<char>(file_stream), std::istreambuf_iterator<char>() };
	std::string file_content = iss.str();
	const GLchar* file_data = file_content.c_str();
	GLint file_length = file_content.length();
	GLuint shader = glCreateShader();
	glShaderSource(shader, 1, &file_data, &file_length);
}

CRenderer::CRenderer()
{
	GLuint vert_shader_world = createShader("../src/visualization/world_vert.glsl");
	GLuint frag_shader_world = createShader("../src/visualization/world_frag.glsl");
	shader_program_world = glCreateProgram();
	glAttachShader(shader_program_world, vert_shader_world);
	glAttachShader(shader_program_world, frag_shader_world);
	glCompileProgram(shader_program_world);
}

void CRenderer::renderWorldObject(const CRenderObject& obj)
{
	world_objects.add(obj);
}

void CRenderer::renderUIObject(const CRenderObject& obj)
{
	ui_objects.add(obj);
}

void CRenderer::present()
{
	glUseProgram(shader_program_world);
	for (const CRenderObject& obj : world_objects) {
		// render obj
	}
	world_objects.clear();

	glUseProgram(shader_program_ui);
	for (const CRenderObject& obj : ui_objects) {
		// render 
	}
	ui_objects.clear();
}