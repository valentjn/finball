#ifndef ARRAY_2D_HPP_
#define ARRAY_2D_HPP_

#include <assert.h>

template <typename T> class Array2D {
  private:
    int width;
    int height;
    T *data;

  public:
    Array2D(int width, int height) : width(width), height(height) {
        data = new T[width * height];
    }

    ~Array2D() { delete[] data; }

    void loadData(void *data) {
        memcpy(this->data, data, sizeof(T) * width * height);
    }

    void setValue(int x, int y, T value) {
        assert(x >= 0 && y >= 0 && y < height && x < width);

        data[y * width + x] = value;
    }

    T getValue(int x, int y) {
        assert(x >= 0 && y >= 0 && y < height && x < width);

        return data[y * width + x];
    }

    T &getRef(int x, int y) {
        assert(x >= 0 && y >= 0 && y < height && x < width);

        return data[y * width + x];
    }

    T *getRawData() { return data; }
};

#endif /* ARRAY2D_HPP_ */
