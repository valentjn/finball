#ifndef MESH_HPP_
#define MESH_HPP_

#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <memory>

#include <Visualization/Texture.hpp>

class Mesh
{
protected:
    GLuint m_vao;
    GLuint m_vbo;
    GLsizei m_vertex_count;

public:
    Mesh(GLsizei vertex_count);
    virtual ~Mesh();
    virtual void render(GLint mode_location) const = 0;
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    bool operator==(const Mesh& other) const;

    static std::vector<glm::vec3> createRectangle(glm::vec2 min, glm::vec2 max);
    static std::vector<glm::vec3> createCircle(glm::vec2 center, float radius, int polygons = 48);
    static std::vector<glm::vec3> createArrow(float scale);
};

class ColoredMesh : public Mesh
{
    struct Vertex {
        glm::vec3 position;
        glm::vec3 color;
    };

    void init(const std::vector<Vertex>& vertex_buffer);

public:
    ColoredMesh(const std::vector<glm::vec3>& vertices, glm::vec3 color);
    ColoredMesh(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& colors);
    void render(GLint mode_location) const final;
};

class TexturedMesh : public Mesh
{
    struct Vertex {
        glm::vec3 position;
        glm::vec2 tex_coord;
    };

    const Texture* m_texture;

    void init(const std::vector<Vertex>& vertex_buffer);

public:
    TexturedMesh(const std::vector<glm::vec3>& vertices, const Texture* texture);
    TexturedMesh(const std::vector<glm::vec3>& vertices, const Texture* texture, const std::vector<glm::vec2> tex_coords);
    void render(GLint mode_location) const final;
    const Texture& getTexture() const;
    void setTexture(const Texture& texture);
};

#endif // MESH_HPP_
