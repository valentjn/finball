#include <visualization/CRenderer.hpp>
#include <glm/glm.hpp>
#include <vector>

#include <iostream>

CMesh::CMesh()
{
	struct Vertex
	{
		glm::vec3 position;
	};

	std::vector<Vertex> vertices{
		{ { -0.5f, -0.5f, 0.f } },
		{ { -0.5f, 0.5f, 0.f } },
		{ { 0.5f, 0.5f, 0.f } },
		{ { -0.5f, -0.5f, 0.f } },
		{ { 0.5f, 0.5f, 0.f } },
		{ { 0.5f, -0.5f, 0.f } } };

	m_vertex_count = static_cast<decltype(m_vertex_count)>(vertices.size());

	// create vertex array object
	m_vao = 0;
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// create and fill vertex buffer object
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER,
		static_cast<GLsizei>(sizeof(Vertex) * vertices.size()),	// data size
		vertices.data(),										// data pointer
		GL_STATIC_DRAW); // hint that the data won't be updated

	std::cout << "1 : " << glGetError() << '\n';


	std::cout << "2 : " << glGetError() << '\n';

	// attach the vertex buffer to the vertex array object
	glVertexAttribPointer(
		0,
		3, GL_FLOAT,					// a position consists of 3 floats
		GL_FALSE,						// is not normalized
		0,								// stride between consecutive position attributes in the buffer
		offsetof(Vertex, position));	// offset (in bytes) of the position attribute in the Vertex struct
	glEnableVertexAttribArray(0);

	std::cout << "3 : " << glGetError() << '\n';

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

CMesh::~CMesh()
{
	glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);
}

void CMesh::render()
{
	std::cout << "4 : " << glGetError() << '\n';
	glBindVertexArray(m_vao);
	std::cout << "5 : " << glGetError() << '\n';
	glDrawArrays(GL_TRIANGLES, 0, m_vertex_count);
	std::cout << "6 : " << glGetError() << '\n';
	glBindVertexArray(0);
	std::cout << "7 : " << glGetError() << '\n';
}
