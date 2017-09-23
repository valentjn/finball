#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>
#include <vector>

#include <renderer/Mesh.hpp>

Mesh::Mesh() : m_vao(0), m_vbo(0), m_vertex_count(0) {}

Mesh::Mesh(const std::vector<Vertex>& vertices)
{
    m_vertex_count = static_cast<decltype(m_vertex_count)>(vertices.size());

    // create vertex array object
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // create and fill vertex buffer object
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizei>(sizeof(Vertex) * vertices.size()), // data size
                 vertices.data(),                                        // data pointer
                 GL_STATIC_DRAW); // hint that the data won't be updated

    // attach the vertex buffer to the vertex array object
    glVertexAttribPointer(0, 3, GL_FLOAT, // a position consists of 3 floats
                          GL_FALSE,       // is not normalized
                          0, // stride between consecutive position attributes in the buffer
                          offsetof(Vertex, position)); // offset (in bytes) of the position
                                                       // attribute in the Vertex struct
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

Mesh::Mesh(Mesh&& other)
	: m_vao(other.m_vao)
	, m_vbo(other.m_vbo)
	, m_vertex_count(other.m_vertex_count)
{
	other.m_vao = 0;
	other.m_vbo = 0;
	other.m_vertex_count = 0;
}

Mesh& Mesh::operator=(Mesh&& other)
{
	Mesh(std::move(*this));
	m_vao = other.m_vao;
	m_vbo = other.m_vbo;
	m_vertex_count = other.m_vertex_count;
	other.m_vao = 0;
	other.m_vbo = 0;
	other.m_vertex_count = 0;
	return *this;
}

void Mesh::render() const
{
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, m_vertex_count);
    glBindVertexArray(0);
}

Mesh createRectangleMesh(float width, float height)
{
	glm::vec3 bl{ -0.5f * width, -0.5f * height, 0.0f };
	glm::vec3 tl{ -0.5f * width, 0.5f * height, 0.0f };
	glm::vec3 br{ 0.5f * width, -0.5f * height, 0.0f };
	glm::vec3 tr{ 0.5f * width, 0.5f * height, 0.0f };
	std::vector<Vertex> vertices{ {bl}, {br}, {tr}, {bl}, {tr}, {tl} };
	return Mesh(vertices);
}

Mesh createCircleMesh(float radius)
{
	std::vector<Vertex> vertices;
    glm::vec3 center{0, 0, 0};
    glm::vec3 rad{radius, 0, 0};
    glm::vec3 normal{0, 0, 1};

    constexpr int ticks = 64;

    float angle;
    float next_angle = 0;

    for (int i = 0; i < ticks; i++) {
        angle = next_angle;
        next_angle = 2 * glm::pi<float>() * (i + 1) / ticks;
        glm::vec3 p2 = glm::rotate(rad, angle, normal);
        glm::vec3 p3 = glm::rotate(rad, next_angle, normal);
        vertices.push_back({center});
        vertices.push_back({p2});
        vertices.push_back({p3});
    }

	return Mesh(vertices);
}

Mesh createArrowMesh(float scale) {
    std::vector<Vertex> vertices {
            {{  0.25f * scale,  0.25f * scale, 0.0f }},
            {{  0.5f  * scale,  0.0f  * scale, 0.0f }},
            {{  0.25f * scale, -0.25f * scale, 0.0f }},

            {{ -0.5f  * scale,  0.15f * scale, 0.0f }},
            {{  0.25f * scale,  0.15f * scale, 0.0f }},
            {{  0.25f * scale, -0.15f * scale, 0.0f }},

            {{ -0.5f  * scale,  0.15f * scale, 0.0f }},
            {{  0.25f * scale, -0.15f * scale, 0.0f }},
            {{ -0.5f  * scale, -0.15f * scale, 0.0f }}
    };

    return Mesh(vertices);
}

bool Mesh::operator==(const Mesh& other) const
{
	return m_vao == other.m_vao;
}

