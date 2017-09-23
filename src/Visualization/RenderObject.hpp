#ifndef RENDER_OBJECT_HPP_
#define RENDER_OBJECT_HPP_

#include <glm/glm.hpp>
#include <renderer/Mesh.hpp>

struct RenderObject {
	Mesh* mesh;
    glm::vec3 position;
    float scale;
	float rotation;
};

#endif // RENDEROBJECT_HPP_
