#ifndef ARRAY_2D_HPP_
#define ARRAY_2D_HPP_

#include <cassert>

template <typename T> class Array2D {
private:
    int m_width;
    int m_height;
    std::vector<T> m_data;

public:
    Array2D(int width, int height) : m_width(width), m_height(height), m_data(width * height) {}

    void loadData(void *data) {
        T *t_data = static_cast<T *>(data);
        std::copy(t_data, t_data + m_data.size(), m_data.begin());
    }

    T &value(int x, int y) {
        assert(x >= 0);
        assert(y >= 0);
        assert(x < m_width);
        assert(y < m_height);
        return m_data[y * m_width + x];
    }

    const T &value(int x, int y) const {
        assert(x >= 0);
        assert(y >= 0);
        assert(x < m_width);
        assert(y < m_height);
        return m_data[y * m_width + x];
    }

    int width() const { return m_width; }
    int height() const { return m_height; }

    const T *getData() { return m_data.data(); }
};

#endif /* ARRAY2D_HPP_ */
