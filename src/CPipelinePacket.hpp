#ifndef CPIPELINEPACKET_HPP_
#define CPIPELINEPACKET_HPP_

#include <typeinfo>

/**
 * packaging for pipeline packets.
 *
 * this class is inherited to other classes for packaging them.
 * Then only this class has to be considered for the pipelining
 * while the receiving pipeline stage is responsible for unpacking
 * the packet based on the packet id.
 */
class CPipelinePacket
{
public:
	/**
	 * packet identifier
	 */
	std::string type_info_name;

	/**
	 * constructor
	 */
	CPipelinePacket()
	{
	}

	/**
	 * deconstructor
	 */
	virtual ~CPipelinePacket()
	{
	}

	/**
	 * set the packet id to know how to unpack payload
	 */
	inline void setPacketTypeInfoName(
			const char *i_type_info_name
			)
	{
		type_info_name = i_type_info_name;
	}

	/**
	 * return the packet id of the payload
	 */
	inline std::string &getPacketTypeInfo()
	{
		return type_info_name;
	}


	/**
	 * return the payload:
	 *
	 * this should be overloaded by the packed class returning a pointer to its instance
	 */
	virtual void *getPayloadRaw() = 0;


	/**
	 * return the payload casted to T
	 */
	template <typename T>
	T *getPayload()
	{
		return (T*)getPayloadRaw();
	}
};

#endif /* CPIPELINEPACKET_HPP_ */
