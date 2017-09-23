#ifndef MESH_HPP_
#define MESH_HPP_

#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>

struct Vertex {
    glm::vec3 position;
};

class Mesh {
    GLuint m_vao;
    GLuint m_vbo;
    GLsizei m_vertex_count;

public:
    Mesh();
    Mesh(const std::vector<Vertex>& vertices);
    ~Mesh();
    Mesh(Mesh&& other);
    Mesh& operator=(Mesh&& other);
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    void render() const;
    bool operator==(const Mesh& other) const;
};

Mesh createRectangleMesh(float width, float height);
Mesh createCircleMesh(float radius);
Mesh createFluidMesh(float width, float height);
Mesh createArrowMesh(float scale);

#endif // MESH_HPP_
