#ifndef cameras_actinometer_hpp
#define cameras_actinometer_hpp

#include "../camera.hpp"

class Actinometer : public Camera {
    Vec3 o, d;

public:
    Actinometer(Vec3 o, Vec3 t);
    Ray apply(const Vec2 &t) override;
};

#ifdef ARC_IMPLEMENTATION

Actinometer::Actinometer(Vec3 o, Vec3 t) : o(o), d((t - o).norm()) {}
Ray Actinometer::apply(const Vec2 &t) {
    return {o, d};
}

#endif
#endif /* cameras_actinometer_hpp */
