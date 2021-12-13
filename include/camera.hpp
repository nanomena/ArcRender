#ifndef camera_hpp
#define camera_hpp

#include "utils.hpp"
#include "vecmath.hpp"

class Camera {

public:
    virtual Ray apply(const Vec2 &t);
};

#ifdef ARC_IMPLEMENTATION

Ray Camera::apply(const Vec2 &t) {
    throw invalid_argument("NotImplementedError");
}

#endif
#endif /* camera_hpp */
