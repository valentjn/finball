#ifndef COMPRENDERER_HPP_
#define COMPRENDERER_HPP_

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include <renderer/Mesh.hpp>
#include <renderer/RenderObject.hpp>
#include <Array2D.hpp>
#include <Level.hpp>

class CompRenderer
{
private:
	std::vector<RenderObject> m_world_objects;
	std::vector<RenderObject> m_ui_objects;

	GLuint m_shader_program_world;
	GLuint m_shader_program_ui;
	GLuint m_shader_program_fluid;
	GLuint m_tex_fluid;
    GLuint m_tex_noise;

	glm::ivec2 m_resolution;
	SDL_Window* m_window;
	SDL_GLContext m_glcontext;

	glm::vec3 m_camera_pos;

    const Array2D<glm::vec3>* m_fluid_vecs;

	// WIP
	std::unique_ptr<Mesh> m_rectangle;

    void render(const RenderObject& object, GLint model_location) const;

public:
    // Currently, the renderer doesn't have output that can be accessed by other components
    struct OutputData {};

    bool update(OutputData& output);

	CompRenderer(const Level& level);
	~CompRenderer();
	CompRenderer (const CompRenderer &obj) = delete;
	CompRenderer & operator= (const CompRenderer & other) = delete;

	void renderWorldObject(const RenderObject& obj);
	void renderUIObject(const RenderObject& obj);
    void setFluidVecs(const Array2D<glm::vec3>& fluid_vecs);
};

#endif
