#ifndef CDATADRAWINGINFORMATION_HPP_
#define CDATADRAWINGINFORMATION_HPP_

#include <typeinfo>
#include "CPipelinePacket.hpp"

class CDataDrawingInformation	:
		public CPipelinePacket
{
public:
	enum
	{
		CIRCLE
	};

	double x, y;
	double size;

	class
	{
	public:
		unsigned char r;
		unsigned char g;
		unsigned char b;
	} color;

	CDataDrawingInformation()	:
		x(-1),
		y(-1),
		size(10)
	{
		color.r = 0;
		color.g = 0;
		color.b = 0;
		CPipelinePacket::setPacketTypeInfoName(typeid(*this).name());
	}

	virtual ~CDataDrawingInformation()
	{
	}

	/**
	 * return the payload:
	 */
	void *getPayloadRaw()
	{
		return this;
	}
};

#endif /* CDRAWINGINFORMATION_HPP_ */
