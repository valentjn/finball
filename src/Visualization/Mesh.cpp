#include <iostream>
#include <vector>
#include <cassert>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/rotate_vector.hpp>

#ifdef OPENCV_LIBS
#include <opencv2/opencv.hpp>
#endif

#include "Log.hpp"
#include "Visualization/Mesh.hpp"

Mesh::Mesh(GLsizei vertex_count) : m_vertex_count(vertex_count) {}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

bool Mesh::operator==(const Mesh& other) const
{
	return m_vao == other.m_vao;
}

std::vector<glm::vec3> Mesh::createRectangle(glm::vec2 min, glm::vec2 max)
{
	glm::vec3 bl{ min.x, min.y, 0.0f };
	glm::vec3 tl{ min.x, max.y, 0.0f };
	glm::vec3 br{ max.x, min.y, 0.0f };
	glm::vec3 tr{ max.x, max.y, 0.0f };
    return std::vector<glm::vec3>{ bl, br, tr, bl, tr, tl };
}
std::vector<glm::vec3> Mesh::createCircle(glm::vec2 center, float radius, int polygons)
{
	std::vector<glm::vec3> vertices;
    glm::vec3 cent;
    glm::vec3 rad{radius, 0, 0};
    glm::vec3 normal{0, 0, 1};
    glm::vec3 p2;
    glm::vec3 p3 = rad;
    for (int i = 0; i < polygons; i++) {
        p2 = p3;
        float next_angle = 2 * glm::pi<float>() * (i + 1) / polygons;
        p3 = glm::rotate(rad, next_angle, normal);
        vertices.push_back({cent});
        vertices.push_back({p2});
        vertices.push_back({p3});
    }
	return vertices;
}

std::vector<glm::vec3> Mesh::createArrow(float scale)
{
    return std::vector<glm::vec3>{
        {  0.25f * scale,  0.25f * scale, 0.0f },
        {  0.5f  * scale,  0.0f  * scale, 0.0f },
        {  0.25f * scale, -0.25f * scale, 0.0f },

        { -0.5f  * scale,  0.15f * scale, 0.0f },
        {  0.25f * scale,  0.15f * scale, 0.0f },
        {  0.25f * scale, -0.15f * scale, 0.0f },

        { -0.5f  * scale,  0.15f * scale, 0.0f },
        {  0.25f * scale, -0.15f * scale, 0.0f },
        { -0.5f  * scale, -0.15f * scale, 0.0f }
    };
}

#ifdef OPENCV_LIBS
// Returns a Textured Mesh with the specified formatted text
// The corresponding texture is saved to textureOut
std::unique_ptr<Mesh> Mesh::createTextMesh(const char *text, std::unique_ptr<Texture4F> &textureOut, glm::ivec3 color,
		glm::ivec4 bgColor, float fontScale, int lineThickness, bool antiAliasing) {
    int font = cv::FONT_HERSHEY_SIMPLEX;
    int lineType = antiAliasing ? cv::LINE_AA : cv::LINE_8;
    int BORDER = 10;
    cv::Size textSize = cv::getTextSize(text, font, fontScale, lineThickness, NULL);

    cv::Mat mat(textSize.height + BORDER*2, textSize.width + BORDER*2, CV_8UC4, cv::Scalar(bgColor.z, bgColor.y, bgColor.x, bgColor.w));
    cv::putText(mat, text, cv::Point(BORDER, textSize.height+BORDER), font, fontScale, cv::Scalar(color.z, color.y, color.x), lineThickness, lineType, false);

    glm::ivec2 textSizeVec(textSize.width+BORDER*2, textSize.height+BORDER*2);
    if (textureOut == nullptr || textureOut->size() != textSizeVec) {
        textureOut = std::make_unique<Texture4F>(textSizeVec, false);
    }
    textureOut->setData(mat);

	auto textRect = Mesh::createRectangle(glm::vec2{-1, -1}, glm::vec2{1, 1});
	auto mesh = std::make_unique<TexturedMesh>(textRect, textureOut.get());

    return mesh;
}

std::unique_ptr<Mesh> Mesh::createImageMesh(const char *filePath, std::unique_ptr<Texture4F> &textureOut, int size) {
    cv::Mat image = cv::imread(filePath, cv::IMREAD_UNCHANGED);
    if (!image.data) {
        Log::error("Image %s not found!", filePath);
        return nullptr;
    } else {
        textureOut = std::make_unique<Texture4F>(glm::ivec2{image.cols, image.rows});
        textureOut->setData(image);
        return std::make_unique<TexturedMesh>(size, textureOut.get());
    }
}
#endif

void ColoredMesh::init(const std::vector<Vertex>& vertex_buffer)
{
    // create vertex array object
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // create and fill vertex buffer object
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizei>(sizeof(Vertex) * vertex_buffer.size()), // data size
        vertex_buffer.data(),                                        // data pointer
        GL_STATIC_DRAW); // hint that the data won't be updated

    // attach the vertex buffer to the vertex array object
    glVertexAttribPointer(
        0,              // attribute index
        3, GL_FLOAT,    // a position consists of 3 floats
        GL_FALSE,       // is not normalized
        sizeof(Vertex), // stride between consecutive position attributes in the buffer
        reinterpret_cast<void*>(offsetof(Vertex, position))); // offset (in bytes) of the position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        1,
        3, GL_FLOAT,
        GL_TRUE,
        sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, color)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

ColoredMesh::ColoredMesh(const std::vector<glm::vec3>& vertices, glm::vec3 color)
    : Mesh(static_cast<decltype(m_vertex_count)>(vertices.size()))
{
    std::vector<Vertex> vertex_buffer(m_vertex_count);
    for (int i = 0; i < m_vertex_count; ++i)
        vertex_buffer[i] = { vertices[i], color };
    init(vertex_buffer);
}

ColoredMesh::ColoredMesh(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& colors)
    : Mesh(static_cast<decltype(m_vertex_count)>(vertices.size()))
{
    assert(vertices.size() == colors.size());
    std::vector<Vertex> vertex_buffer(m_vertex_count);
    for (int i = 0; i < m_vertex_count; ++i)
        vertex_buffer[i] = { vertices[i], colors[i] };
    init(vertex_buffer);
}

void ColoredMesh::render(GLint mode_location) const
{
    glBindVertexArray(m_vao);
    glUniform1i(mode_location, 0); // colored mode: 0
    glDrawArrays(GL_TRIANGLES, 0, m_vertex_count);
    glBindVertexArray(0);
}

void TexturedMesh::init(const std::vector<Vertex>& vertex_buffer)
{
    // create vertex array object
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // create and fill vertex buffer object
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizei>(sizeof(Vertex) * vertex_buffer.size()), // data size
        vertex_buffer.data(),                                        // data pointer
        GL_STATIC_DRAW); // hint that the data won't be updated

    // attach the vertex buffer to the vertex array object
    glVertexAttribPointer(
        0,              // attribute index
        3, GL_FLOAT,    // a position consists of 3 floats
        GL_FALSE,       // is not normalized
        sizeof(Vertex), // stride between consecutive position attributes in the buffer
        reinterpret_cast<void*>(offsetof(Vertex, position))); // offset (in bytes) of the position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        1,
        2, GL_FLOAT,
        GL_TRUE,
        sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, tex_coord)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// Creates a square textured mesh
TexturedMesh::TexturedMesh(int scale, const Texture* texture) :
        Mesh(static_cast<decltype(m_vertex_count)>(6)),
        m_texture(texture) {
    std::vector<glm::vec3> vertices = Mesh::createRectangle({-scale, -scale}, {scale, scale});
    std::vector<Vertex> vertex_buffer(6);
    for (int i = 0; i < m_vertex_count; i++) {
        vertex_buffer[i] = {vertices[i], { vertices[i].x > 0 ? 1 : 0, vertices[i].y > 0 ? 1 : 0 }};
    }
    init(vertex_buffer);
}

TexturedMesh::TexturedMesh(const std::vector<glm::vec3>& vertices, const Texture* texture)
    : Mesh(static_cast<decltype(m_vertex_count)>(vertices.size()))
    , m_texture(texture)
{
    std::vector<Vertex> vertex_buffer(m_vertex_count);
    for (int i = 0; i < m_vertex_count; ++i) {
        const glm::vec3& vertex = vertices[i];
        vertex_buffer[i] = { vertex, glm::vec2(vertex) * 0.5f + 0.5f };
    }
    init(vertex_buffer);
}

TexturedMesh::TexturedMesh(const std::vector<glm::vec3>& vertices, const Texture* texture, const std::vector<glm::vec2> tex_coords)
    : Mesh(static_cast<decltype(m_vertex_count)>(vertices.size()))
    , m_texture(texture)
{
    assert(vertices.size() == tex_coords.size());
    std::vector<Vertex> vertex_buffer(m_vertex_count);
    for (int i = 0; i < m_vertex_count; ++i) {
        vertex_buffer[i] = { vertices[i], tex_coords[i] };
    }
    init(vertex_buffer);
}

void TexturedMesh::render(GLint mode_location) const
{
    glBindVertexArray(m_vao);
    glUniform1i(mode_location, 1); // textured mode: 1
    assert(m_texture);
    m_texture->bind(0);
    glDrawArrays(GL_TRIANGLES, 0, m_vertex_count);
    glBindVertexArray(0);
}

const Texture& TexturedMesh::getTexture() const { return *m_texture; }

void TexturedMesh::setTexture(const Texture& texture) { m_texture = &texture; }
