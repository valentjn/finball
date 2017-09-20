#ifndef MESH_HPP_
#define MESH_HPP_

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

class Mesh {
    GLuint m_vao;
    GLuint m_vbo;
    GLsizei m_vertex_count;

  public:
    Mesh();
    ~Mesh();
    void render();
};

#endif // MESH_HPP_
