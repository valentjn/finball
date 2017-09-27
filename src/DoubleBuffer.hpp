#ifndef DOUBLE_BUFFER_HPP_
#define DOUBLE_BUFFER_HPP_

#include <array>
#include <mutex>

using namespace std;

template <typename T>
class DoubleBuffer
{
    array<T, 2> buffers;
    int writeBufferId;
    mutable std::mutex mtx;

public:
	DoubleBuffer() : writeBufferId(0) {}

    void swap()
	{
        mtx.lock();
        writeBufferId = 1 - writeBufferId;
        mtx.unlock();
    }

	bool lock() const
	{
        mtx.lock();
		return true;
    }

    bool unlock() const
	{
        mtx.unlock();
		return true;
    }

    T& writeBuffer() { return buffers[writeBufferId]; }

    const T& readBuffer() const { return buffers[1 - writeBufferId]; }
};

template<>
class DoubleBuffer<void> {};

#endif
