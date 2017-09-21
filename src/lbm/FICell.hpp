#ifndef FI_CELL_HPP_
#define FI_CELL_HPP_

struct FICell {
    float components[9] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};

    float &operator[](int index) {
        assert(index >= 0 && index < 9);
        return components[index];
    }
};

#endif
