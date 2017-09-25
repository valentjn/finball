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
#include "Visualization/Texture.hpp"
#include "SDL/SDLWindow.hpp"

class Renderer {
private:
    constexpr static int m_fluid_width = 256;
    constexpr static int m_fluid_height = 256;

    std::vector<RenderObject> m_world_objects;
    std::vector<RenderObject> m_ui_objects;

    GLuint m_shader_program_world;
    GLuint m_shader_program_ui;
    GLuint m_shader_program_fluid;

    std::unique_ptr<Texture3F> m_tex_fluid_input;
	std::unique_ptr<Texture1F> m_tex_waves1;
	std::unique_ptr<Texture1F> m_tex_waves2;
    std::unique_ptr<Texture1F> m_tex_noise;
    std::unique_ptr<Texture3F> m_tex_fluid_output;

    std::array<GLuint, 2> m_framebuffers_fluid_output;
	bool m_inter_fb1;

    glm::ivec2 m_resolution;
    SDL_Window *m_window;
    SDL_GLContext m_glcontext;

    glm::vec3 m_camera_pos;

    const Array2D<glm::vec3> *m_fluid_vecs;

	std::unique_ptr<Mesh> m_full_quad;

    void render(const RenderObject &object, GLint model_location, GLint mode_location) const;

public:
    void update(const RendererInput &input);

    Renderer(const SDLWindow &window);
    ~Renderer();
    Renderer(const Renderer &obj) = delete;
    Renderer &operator=(const Renderer &other) = delete;
};

#endif
