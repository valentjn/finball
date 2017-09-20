#ifndef RENDERER_HPP_
#define RENDERER_HPP_

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "Parameters.hpp"
#include "RendererInput.hpp"
#include "Mesh.hpp"
#include "RenderObject.hpp"

class Renderer {
private:
    Parameters &parameters;

	GLuint m_shader_program_world;
	GLuint m_shader_program_ui;

	glm::ivec2 m_resolution;
	SDL_Window *m_window;
	SDL_GLContext m_glcontext;

	glm::vec3 m_camera_pos;

	// WIP
	std::unique_ptr<Mesh> m_rectangle;

    void render(const RenderObject &object, GLint model_location) const;

public:
    void update(const RendererInput &input);

	Renderer(Parameters &parameters);
	~Renderer();

	void renderWorldObject(const RenderObject &obj);
	void renderUIObject(const RenderObject &obj);
};

#endif
