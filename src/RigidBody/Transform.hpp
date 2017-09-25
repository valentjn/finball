#ifndef TRANSFORM_HPP_
#define TRANSFORM_HPP_

class Transform {
public:
    int id;
    glm::vec2 position;
    float rotation;
    Transform(int id, glm::vec2 position, float rotation) :
        id(id),
        position(position),
        rotation(rotation)
    {}
};

#endif
