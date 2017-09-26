#ifndef DOUBLE_BUFFER_HPP_
#define DOUBLE_BUFFER_HPP_

#include <mutex>

using namespace std;

template <typename T>
class DoubleBuffer {

private:
    T buffers[2];
    int writeBufferId = 0;
    std::mutex mtx;

public:
    template<class... Args>
    DoubleBuffer(Args&&... args) {
        buffers[0] = T(forward<Args>(args)...);
        buffers[1] = T(forward<Args>(args)...);
    }

    void swap() {
        mtx.lock();
        writeBufferId = (writeBufferId + 1) % 2;
        mtx.unlock();
    }

    void lock() {
        mtx.lock();
    }

    void unlock() {
        mtx.unlock();
    }

    T &writeBuffer() {
        return buffers[writeBufferId];
    }

    T &readBuffer() {
        return buffers[(writeBufferId + 1) % 2];
    }

};

#endif
