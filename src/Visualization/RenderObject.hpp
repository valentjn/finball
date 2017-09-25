#ifndef RENDER_OBJECT_HPP_
#define RENDER_OBJECT_HPP_

#include <glm/glm.hpp>

#include "Visualization/Mesh.hpp"

struct RenderObject {
	const Mesh* mesh;
    glm::vec3 position;
	float rotation;
    glm::vec2 scale;
};

#endif
