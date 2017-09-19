#ifndef COMPRENDERER_HPP_
#define COMPRENDERER_HPP_

class RenderObject {

};

class CompRenderer
{
private:
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
    struct OutputData {
    };

    void update(OutputData& output);

	CompRenderer();
	~CompRenderer();

	void renderWorldObject(const CRenderObject& obj);
	void renderUIObject(const CRenderObject& obj);
};

#endif
