#include <Visualization/Texture.hpp>

Texture::Texture(glm::ivec2 size) : m_res(size) {}

glm::ivec2 Texture::size() const { return m_res; }

int Texture::sizeX() const { return m_res.x; }

int Texture::sizeY() const { return m_res.y; }

GLuint Texture::texture() { return m_tex; }

void Texture::bind(int index) const
{
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, m_tex);
}

Texture1F::Texture1F(glm::ivec2 size)
    : Texture(size)
{
    glGenTextures(1, &m_tex);
    bind(0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_res.x, m_res.y, 0, GL_LUMINANCE, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void Texture1F::setData(const Array2D<float>& data)
{
    assert(data.width() == m_res.x);
    assert(data.height() == m_res.y);
    bind(0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_res.x, m_res.y, 0, GL_LUMINANCE, GL_FLOAT, data.getData()); // TODO: GL_R ?
}

Texture2F::Texture2F(glm::ivec2 size)
    : Texture(size)
{
    glGenTextures(1, &m_tex);
    bind(0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_res.x, m_res.y, 0, GL_RG, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void Texture2F::setData(const Array2D<glm::vec2>& data)
{
    assert(data.width() == m_res.x);
    assert(data.height() == m_res.y);
    bind(0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_res.x, m_res.y, 0, GL_RG, GL_FLOAT, data.getData());
}

Texture3F::Texture3F(glm::ivec2 size)
    : Texture(size)
{
    glGenTextures(1, &m_tex);
    bind(0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_res.x, m_res.y, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void Texture3F::setData(const Array2D<glm::vec3>& data)
{
    assert(data.width() == m_res.x);
    assert(data.height() == m_res.y);
    bind(0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_res.x, m_res.y, 0, GL_RGB, GL_FLOAT, data.getData());
}

Texture4F::Texture4F(glm::ivec2 size)
    : Texture(size)
{
    glGenTextures(1, &m_tex);
    bind(0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_res.x, m_res.y, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void Texture4F::setData(const Array2D<glm::vec4>& data)
{
    assert(data.width() == m_res.x);
    assert(data.height() == m_res.y);
    bind(0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_res.x, m_res.y, 0, GL_RGBA, GL_FLOAT, data.getData());
}

