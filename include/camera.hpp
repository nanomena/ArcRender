#ifndef camera_hpp
#define camera_hpp

#include "utils.hpp"
#include "vecmath.hpp"

class Camera {

public:
    virtual Ray apply(const Vec2 &t) const {
        throw invalid_argument("NotImplementedError");
    }
};

#ifdef ARC_IMPLEMENTATION

#endif
#endif /* camera_hpp */
