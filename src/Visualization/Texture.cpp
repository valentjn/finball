#include <Visualization/Texture.hpp>
#include <Log.hpp>

Texture::Texture(glm::ivec2 size, bool linear_interpolation)
    : m_res(size)
{
    glGenTextures(1, &m_tex);
    bind(0);
    if (linear_interpolation) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
}

glm::ivec2 Texture::size() const { return m_res; }

int Texture::sizeX() const { return m_res.x; }

int Texture::sizeY() const { return m_res.y; }

GLuint Texture::texture() { return m_tex; }

void Texture::bind(int index) const
{
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, m_tex);
}

Texture1F::Texture1F(glm::ivec2 size, bool linear_interpolation)
    : Texture(size, linear_interpolation)
{
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_res.x, m_res.y, 0, GL_RED, GL_FLOAT, nullptr);
}

void Texture1F::setData(const Array2D<float>& data)
{
    assert(data.width() == m_res.x);
    assert(data.height() == m_res.y);
    bind(0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_res.x, m_res.y, 0, GL_RED, GL_FLOAT, data.getData()); // TODO: GL_R ?
}

Texture2F::Texture2F(glm::ivec2 size, bool linear_interpolation)
    : Texture(size, linear_interpolation)
{
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, m_res.x, m_res.y, 0, GL_RG, GL_FLOAT, nullptr);
}

void Texture2F::setData(const Array2D<glm::vec2>& data)
{
    assert(data.width() == m_res.x);
    assert(data.height() == m_res.y);
    bind(0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, m_res.x, m_res.y, 0, GL_RG, GL_FLOAT, data.getData());
}

Texture3F::Texture3F(glm::ivec2 size, bool linear_interpolation)
    : Texture(size, linear_interpolation)
{
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_res.x, m_res.y, 0, GL_RGB, GL_FLOAT, nullptr);
}

void Texture3F::setData(const Array2D<glm::vec3>& data)
{
    assert(data.width() == m_res.x);
    assert(data.height() == m_res.y);
    bind(0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_res.x, m_res.y, 0, GL_RGB, GL_FLOAT, data.getData());
}

Texture4F::Texture4F(glm::ivec2 size, bool linear_interpolation)
    : Texture(size, linear_interpolation)
{
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_res.x, m_res.y, 0, GL_RGBA, GL_FLOAT, nullptr);
}

void Texture4F::setData(const Array2D<glm::vec4>& data)
{
    assert(data.width() == m_res.x);
    assert(data.height() == m_res.y);
    bind(0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_res.x, m_res.y, 0, GL_RGBA, GL_FLOAT, data.getData());
}

#ifdef OPENCV_LIBS
void Texture4F::setData(const cv::Mat &data) {
    assert(data.cols == m_res.x);
    assert(data.rows == m_res.y);
    bind(0);
    // mirror the image around the x axis (OpenCV and OpenGL store images differently)
	cv::flip(data, data, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_res.x, m_res.y, 0, GL_BGRA, GL_UNSIGNED_BYTE, data.data);
}

void Texture4F::createText(
	std::unique_ptr<Texture4F>& texture,
	const char *text,
	glm::vec4 color, glm::vec4 bgColor,
	float fontScale, int lineThickness, bool antiAliasing)
{
	int font = cv::FONT_HERSHEY_SIMPLEX;
	int lineType = antiAliasing ? cv::LINE_AA : cv::LINE_8;
	int BORDER = 10;
	cv::Size textSize = cv::getTextSize(text, font, fontScale, lineThickness, NULL);

	color *= 255;
	bgColor *= 255;
	cv::Mat mat(
		textSize.height + BORDER * 2,
		textSize.width + BORDER * 2,
		CV_8UC4,
		cv::Scalar(bgColor.z, bgColor.y, bgColor.x, bgColor.w));
	cv::putText(
		mat,
		text,
		cv::Point(BORDER, textSize.height + BORDER),
		font, fontScale,
		cv::Scalar(color.z, color.y, color.x, color.w),
		lineThickness, lineType);
	glm::ivec2 textSizeVec(textSize.width + BORDER * 2, textSize.height + BORDER * 2);

	if (!texture || texture->size() != textSizeVec)
		texture = std::make_unique<Texture4F>(textSizeVec, false);

	texture->setData(mat);
}

void Texture4F::createImage(
	std::unique_ptr<Texture4F>& texture,
	const char *filePath)
{
	cv::Mat image = cv::imread(filePath, cv::IMREAD_UNCHANGED);

	if (!image.data) {
		Log::error("Image %s not found!", filePath);
		return;
	}

	if (!texture || texture->size() != glm::ivec2{ image.cols, image.rows })
		texture = std::make_unique<Texture4F>(glm::ivec2{ image.cols, image.rows });

	texture->setData(image);
}

#endif
