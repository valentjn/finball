#ifndef COMPRENDERER_HPP_
#define COMPRENDERER_HPP_

#include <memory>
#include <vector>

#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include "Array2D.hpp"
#include "Level.hpp"
#include "Visualization/Mesh.hpp"
#include "Visualization/RenderObject.hpp"
#include "Visualization/RendererInput.hpp"

class Renderer {
private:
    std::vector<RenderObject> m_world_objects;
    std::vector<RenderObject> m_ui_objects;

    GLuint m_shader_program_world;
    GLuint m_shader_program_ui;
    GLuint m_shader_program_fluid;
    GLuint m_tex_fluid;
    GLuint m_tex_noise;

    glm::ivec2 m_resolution;
    SDL_Window *m_window;
    SDL_GLContext m_glcontext;

    glm::vec3 m_camera_pos;

    const Array2D<glm::vec3> *m_fluid_vecs;

	// WIP
	Mesh m_full_quad;

    void render(const RenderObject &object, GLint model_location) const;

public:
    void update(const RendererInput &input);

    Renderer();
    ~Renderer();
    Renderer(const Renderer &obj) = delete;
    Renderer &operator=(const Renderer &other) = delete;
};

#endif
