#ifndef CMESH_HPP_
#define CMESH_HPP_

class CMesh
{
	GLuint m_vao;
	GLuint m_vbo;
	GLsizei m_vertex_count;

public:
	CMesh();
	~CMesh();
	void render();
};

#endif // CMESH_HPP_