#ifndef ARRAY_2D_HPP_
#define ARRAY_2D_HPP_

#include <cassert>
#include <vector>

template <typename T>
class Array2D {
private:
    int m_width;
    int m_height;
    std::vector<T> m_data;

public:
    Array2D() : m_width(0), m_height(0) {}

    Array2D(int width, int height) : m_width(width), m_height(height), m_data(width * height) {}

    Array2D(const Array2D& other)
        : m_width(other.m_width), m_height(other.m_height), m_data(other.m_data)
    {}

    Array2D& operator=(const Array2D& other)
    {
        m_width = other.m_width;
        m_height = other.m_height;
        m_data = other.m_data;
    }

    Array2D(Array2D &&other)
        : m_width(other.m_width), m_height(other.m_height), m_data(std::move(other.m_data)) {
        other.m_width = 0;
        other.m_height = 0;
    }

    Array2D &operator=(Array2D &&other) {
        m_width = other.m_width;
        m_height = other.m_height;
        m_data = std::move(other.m_data);
        other.m_width = 0;
        other.m_height = 0;
        return *this;
    }

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

    T *getData() { return m_data.data(); }
    const T *getData() const { return m_data.data(); }

    bool operator==(const Array2D<T>& other) const
    {
        if (m_width != other.m_width || m_height != other.m_height)
            return false;
        for (int i = 0; i < m_width; ++i)
            for (int j = 0; j < m_height; ++j)
                if (value(i,j) != other.value(i,j))
                    return false;
        return true;
    }
};

#endif /* ARRAY2D_HPP_ */
