/*
 * Copyright 2010 Martin Schreiber
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef C_GL_TEXTURE_HPP
#define C_GL_TEXTURE_HPP

#include "GL/gl.h"
#include <iostream>


/**
 * \brief	general texture handler for 2d, 3d, rectangle and cubemap texture
 */
class CGlTexture
{
public:
	int width;			///< width of texture
	int height;			///< height of texture
	int depth;			///< depth of texture (for 3d textures)

	GLuint textureid;	///< OpenGL texture handler

	GLint int_format;	///< internal texture format
	GLenum ext_format;	///< external texture format to load data from
	GLenum ext_type;	///< external texture format type

	GLenum target;		///< texture target, usually GL_TEXTURE_2D


	/**
	 * setup the texture types, parameters, internal formats, etc.
	 *
	 * this is only relevant for further usage, no OpenGL command is called and no conversions are done
	 */
	inline void setTextureParameters(
						GLenum p_target = GL_TEXTURE_2D,
						GLint p_int_format = GL_RGBA,
						GLenum p_ext_format = GL_RGBA,
						GLenum p_ext_type = GL_UNSIGNED_BYTE
					)
	{
		target = p_target;
		int_format = p_int_format;
		ext_format = p_ext_format;
		ext_type = p_ext_type;
	}



	/**
	 * initialize texture
	 */
	inline CGlTexture(	GLenum p_target = GL_TEXTURE_2D,
						GLint p_int_format = GL_RGBA,
						GLenum p_ext_format = GL_RGBA,
						GLenum p_ext_type = GL_UNSIGNED_BYTE
					)
	{
		glGenTextures(1, &textureid);

		width = 0;
		height = 0;
		depth = 0;

		setTextureParameters(p_target, p_int_format, p_ext_format, p_ext_type);
		setLinearInterpolation();

		setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		setParam(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}



	/**
	 * resize of initialize texture with the given size
	 *
	 * call BIND() before resizing!
	 */
	inline bool resize(int p_width, int p_height)
	{
		/**
		 * free texture data
		 */
		width = p_width;
		height = p_height;

		glTexImage2D(target, 0, int_format, width, height, 0, ext_format, ext_type, 0);

		return true;
	}

	/**
	 * only update the width and height values in this class without modifying the texture itself
	 *
	 * \param p_width	new width value
	 * \param p_height	new height value
	 */
	inline void setSizeParameter(int p_width, int p_height)
	{
		width = p_width;
		height = p_height;
	}

	/**
	 * bind texture to texture unit
	 */
	inline void bind()
	{
		glBindTexture(target, textureid);
	}

	/**
	 * unbind texture target
	 */
	inline void unbind()
	{
		glBindTexture(target, 0);
	}

	/**
	 * set texture GLint parameter
	 */
	inline void setParam(GLenum pname, GLint param)
	{
		bind();
		glTexParameteri(target, pname, param);
		unbind();
	}


	/**
	 * set texture GLfloat[4] parameter
	 */
	inline void setParamfv(GLenum pname, const GLfloat *param)
	{
		glTexParameterfv(target, pname, param);
	}


	/**
	 * set texture GLint[4] parameter
	 */
	inline void setParamiv(GLenum pname, const GLint *param)
	{
		glTexParameteriv(target, pname, param);
	}

	/**
	 * set texture GLenum parameter
	 */
	inline GLfloat getParami(GLenum pname)
	{
		GLfloat param;
		glGetTexParameterfv(target, pname, &param);
		return param;
	}

	/**
	 * return texture GLenum parameter
	 */
	inline GLint getParam(GLenum pname)
	{
		GLint param;
		glGetTexParameteriv(target, pname, &param);
		return param;
	}

	/**
	 * return texture id
	 */
	inline GLuint getTextureId()
	{
		return textureid;
	}

	/**
	 * return internal texture format
	 */
	inline GLenum getFormat()
	{
		return int_format;
	}

	/**
	 * copy the data pointed by data to the texture
	 *
	 * \param data	pointer to new texture data (float array with only one [red] component)
	 */
	inline void setData(void *data)
	{
		switch(target)
		{
			case GL_TEXTURE_2D:
			case GL_TEXTURE_RECTANGLE:
				glTexImage2D(target, 0, int_format, width, height, 0, ext_format, ext_type, data);
				break;

			case GL_TEXTURE_3D:
				glTexImage3D(GL_TEXTURE_3D, 0, int_format, width, height, depth, 0, ext_format, ext_type, data);
				break;

			default:
				std::cerr << "unsupported target (in setData())" << std::endl;
				break;
		}
	}

	/**
	 * setup quickly nearest neighbor filtering
	 */
	inline void setNearestNeighborInterpolation()
	{
		bind();
		setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	/**
	 * setup quickly nearest neighbor filtering
	 */
	inline void setLinearInterpolation()
	{
		bind();
		setParam(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		setParam(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		unbind();
	}

	inline ~CGlTexture()
	{
		glDeleteTextures(1, &textureid);
	}
};

#endif
