#ifndef CRENDERER_HPP_
#define CRENDERER_HPP_

// include this first
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#include <visualization/CRenderObject.hpp>
#include <visualization/CMesh.hpp>
#include <SDL2/SDL.h>
#include <vector>
#include <memory>

class CRenderer
{
	std::vector<CRenderObject> m_world_objects;
	std::vector<CRenderObject> m_ui_objects;

	GLuint m_shader_program_world;
	GLuint m_shader_program_ui;

	glm::ivec2 m_resolution;
	SDL_Window* m_window;
	SDL_GLContext m_glcontext;

	glm::vec3 m_camera_pos;

	// WIP
	std::unique_ptr<CMesh> m_rectangle;

public:
	CRenderer();
	~CRenderer();
	void renderWorldObject(const CRenderObject& obj);
	void renderUIObject(const CRenderObject& obj);
	void present();
};

#endif //CRENDERER_HPP_
