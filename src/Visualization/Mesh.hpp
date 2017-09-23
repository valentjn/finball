#ifndef MESH_HPP_
#define MESH_HPP_

#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <memory>

struct Vertex {
    glm::vec3 position;
};

class Mesh
{
    GLuint m_vao;
    GLuint m_vbo;
    GLsizei m_vertex_count;

public:
    Mesh(const std::vector<Vertex>& vertices);
    virtual ~Mesh();
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    void render() const;
    bool operator==(const Mesh& other) const;
};

class ColoredMesh
{
    ColoredMesh();
    ColoredMesh(const std::vector<Vertex>& vertices, glm::vec3 color);
    ~ColoredMesh();
};

std::unique_ptr<Mesh> createRectangleMesh(float width, float height);
std::unique_ptr<Mesh> createCircleMesh(float radius);
std::unique_ptr<Mesh> createFluidMesh(float width, float height);
std::unique_ptr<Mesh> createArrowMesh(float scale);

#endif // MESH_HPP_
