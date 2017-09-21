#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>
#include <vector>

#include <renderer/Mesh.hpp>
#include "../../ext/glm/glm/vec3.hpp"
#include "../../ext/glm/vec3.hpp"
#include "../../ext/glm/ext.hpp"
#include "../../ext/glm/glm/gtc/matrix_transform.hpp"

struct Vertex {
    glm::vec3 position;
};


std::vector<Vertex> createCircleVertices() {
    std::vector<Vertex> vertices;
    glm::vec3 center{0, 0, 0};
    glm::vec3 radius{1, 0, 0};
    glm::vec3 normal{0, 0, 1};

    // TODO take pi from a library
    constexpr float pi = 3.14;
    constexpr int ticks = 64;

    float angle;
    float next_angle = 0;

    for (int i = 0; i < ticks; i++) {
        angle = next_angle;
        next_angle = 2 * pi * (i + 1) / ticks;
        glm::vec3 p2 = glm::rotate(radius, angle, normal);
        glm::vec3 p3 = glm::rotate(radius, next_angle, normal);
        vertices.push_back({center});
        vertices.push_back({p2});
        vertices.push_back({p3});
    }
    return vertices;
}

std::vector<Vertex> createSquareVertices() {
    return std::vector<Vertex> {
        {{-1.f, -1.f, 0.f}}, {{-1.f, 1.f, 0.f}}, {{1.f,  1.f, 0.f}},
        {{-1.f, -1.f, 0.f}}, {{ 1.f, 1.f, 0.f}}, {{1.f, -1.f, 0.f}}
    };
}


Mesh::Mesh() {
    auto vertices = createCircleVertices();

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

Mesh::~Mesh() {
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

void Mesh::render() {
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, m_vertex_count);
    glBindVertexArray(0);
}
