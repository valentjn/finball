#ifndef CDATAARRAY2D_H_
#define CDATAARRAY2D_H_

#include <typeinfo>
#include <string.h>
#include "CPipelinePacket.hpp"

/**
 * data storage array for type T and D components per array element
 */
template <typename T, int D>
class CDataArray2D	:
		public CPipelinePacket
{
public:
	/**
	 * enumerator for data types to allow casting
	 */
	enum CDataArray2DTypes
	{
		CDATA_ARRAY_2D_CHAR_1,	// e.g. for grey color
		CDATA_ARRAY_2D_CHAR_3,	// e.g. for RBG
		CDATA_ARRAY_2D_FLOAT_1,	// e.g. for density
		CDATA_ARRAY_2D_FLOAT_2,	// e.g. for velocity
		CDATA_ARRAY_2D_FLOAT_3,	// e.g. for 3d velocity
		CDATA_ARRAY_2D_FLOAT_4	// e.g. for multi components
	};

	/**
	 * data array type
	 */
	CDataArray2DTypes type;

	/**
	 * width of array
	 */
	int width;

	/**
	 * height of array
	 */
	int height;

	/**
	 * data storage
	 */
	T *data;

	/**
	 * return true if data is valid
	 */
	bool isValidData()
	{
		return data != nullptr;
	}

	/**
	 * return a pointer of to this instance
	 */
	void *getPayloadRaw()
	{
		return this;
	}

	/**
	 * setup the type id do pack and unpack this data
	 */
	void setupType()
	{
		if (typeid(T) == typeid(char))
		{
			switch(D)
			{
				case 1:	type = CDATA_ARRAY_2D_CHAR_1;	break;
				case 3:	type = CDATA_ARRAY_2D_CHAR_3;	break;
				default:	std::cerr << "Unsupported data array type for type char" << std::endl;	exit(-1);
			}
		}

		if (typeid(T) == typeid(float))
		{
			switch(D)
			{
				case 1:	type = CDATA_ARRAY_2D_FLOAT_1;	break;
				case 2:	type = CDATA_ARRAY_2D_FLOAT_2;	break;
				case 3:	type = CDATA_ARRAY_2D_FLOAT_3;	break;
				case 4:	type = CDATA_ARRAY_2D_FLOAT_4;	break;
				default:	std::cerr << "Unsupported data array type for type float " << std::endl;	exit(-1);
			}
		}
	}


	inline CDataArray2D()	:
		width(-1),
		height(-1),
		data(nullptr)
	{
		setupType();

		CPipelinePacket::setPacketTypeInfoName(typeid(*this).name());
	}


	/**
	 * fill with data
	 */
	inline void loadData(
			void *i_data		///< data to store to this 2d array
	)
	{
		memcpy(data, i_data, sizeof(T)*D*width*height);
	}


	/**
	 * resize the array
	 */
	inline void resize(
			int i_width,	///< new width
			int i_height	///< new height
	)
	{
		cleanup();

		width = i_width;
		height = i_height;
		data = new T[width*height*D];
	}

	/**
	 * return reference to array element
	 */
	inline T &getRef(int i_x, int i_y)
	{
		return data[(i_y*width + i_x)*D];
	}

	/**
	 * return reference to component of array element
	 */
	inline T &getRef(int i_x, int i_y, int i_component)
	{
		return data[(i_y*width + i_x)*D+i_component];
	}

	/**
	 * return reference to component of array element
	 */
	inline T &getClampedRef(int i_x, int i_y)
	{
		return data[(std::min(std::max(i_y,0),height-1)*width + std::min(std::max(i_x,0),width-1))*D];
	}

	/**
	 * return reference to component of array element
	 */
	inline T &getClampedRef(int i_x, int i_y, int i_component)
	{
		return data[(std::min(std::max(i_y,0),height-1)*width + std::min(std::max(i_x,0),width-1))*D+i_component];
	}

	inline void cleanup()
	{
		if (data != nullptr)
		{
			delete []data;
			data = nullptr;
		}

		width = -1;
		height = -1;
	}
	virtual ~CDataArray2D()
	{
		cleanup();
	}
};

#endif /* CDATAARRAY2D_H_ */
