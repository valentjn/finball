#ifndef CRENDERER_HPP_
#define CRENDERER_HPP_

#include <visualization/CRenderObject.hpp>
#include <GL/gl.h>
#include <vector>

class CRenderer
{
	std::vector<CRenderObject> world_objects;
	std::vector<CRenderObject> ui_objects;

	GLuint shader_program_world;
	GLuint shader_program_ui;

public:
	CRenderer();

	void renderWorldObject(const CRenderObject& obj)
	{
		world_objects.push_back(obj);
	}

	void renderUIObject(const CRenderObject& obj)
	{
		ui_objects.push_back(obj);
	}

	void present()
	{
		glUseProgram(shader_program_world);
		for (const CRenderObject& obj : world_objects) {
			// render obj
		}

		glUseProgram(shader_program_ui);
		for (const CRenderObject& obj : ui_objects) {
			// render 
		}
	}
};

#endif //CRENDERER_HPP_