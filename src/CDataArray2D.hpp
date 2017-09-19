#ifndef CDATAARRAY2D_H_
#define CDATAARRAY2D_H_

#include <typeinfo>
#include <string.h>
#include "CPipelinePacket.hpp"

/**
 * data storage array for type T and D components per array element
 */
template <typename T, int D>
class CDataArray2D :
		public CPipelinePacket
{
public:

	


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

	inline CDataArray2D()	:
		width(-1),
		height(-1),
		data(nullptr)
	{
		CPipelinePacket::setPacketTypeInfoName(typeid(*this).name());
	}

	virtual ~CDataArray2D()
	{
		cleanup();
	}

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
	 * fill with data
	 */
	inline void loadData(void *i_data)
	{
		memcpy(data, i_data, sizeof(T)*D*width*height);
	}


	/**
	 * resize the array
	 */
	inline void resize(int i_width,	int i_height)
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
};

#endif /* CDATAARRAY2D_H_ */
