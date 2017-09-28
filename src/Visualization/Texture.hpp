#ifndef TEXTURE_HPP_
#define TEXTURE_HPP_

#include <GL/gl.h>
#include <glm/glm.hpp>
#include <Array2D.hpp>
#ifdef OPENCV_LIBS
#include <opencv2/opencv.hpp>
#endif

class Texture
{
protected:
    glm::ivec2 m_res;
    GLuint m_tex;

    Texture(glm::ivec2 resolution, bool linear_interpolation = true);

public:
    glm::ivec2 size() const;
    int sizeX() const;
    int sizeY() const;
    GLuint texture();
    void bind(int index) const;
};

class Texture1F : public Texture
{
public:
    Texture1F(glm::ivec2 resolution, bool linear_interpolation = true);
    void setData(const Array2D<float>& data);
};

class Texture2F : public Texture
{
public:
    Texture2F(glm::ivec2 resolution, bool linear_interpolation = true);
    void setData(const Array2D<glm::vec2>& data);
};

class Texture3F : public Texture
{
public:
    Texture3F(glm::ivec2 resolution, bool linear_interpolation = true);
    void setData(const Array2D<glm::vec3>& data);
};

class Texture4F : public Texture
{
public:
    Texture4F(glm::ivec2 resolution, bool linear_interpolation = true);
    void setData(const Array2D<glm::vec4>& data);

#ifdef OPENCV_LIBS
    void setData(const cv::Mat& data);

	static void createText(
		std::unique_ptr<Texture4F>& texture,
		const char *text,
		glm::vec4 color = { 1, 1, 1, 1 }, glm::vec4 bgColor = { 0, 0, 0, 0 },
		float fontScale = 2, int lineThickness = 4, bool antiAliasing = true);

	static void createImage(
		std::unique_ptr<Texture4F>& texture,
		const char *filePath);
#endif
};

#endif
