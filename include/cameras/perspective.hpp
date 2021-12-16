#ifndef cameras_perspective_hpp
#define cameras_perspective_hpp

#include "../camera.hpp"

class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera(Vec3 o, Vec3 x, Vec3 y, double zLength);
    Ray apply(const Vec2 &t) const override;

private:
    Vec3 o, x, y, z;
};

#ifdef ARC_IMPLEMENTATION

PerspectiveCamera::PerspectiveCamera(Vec3 o, Vec3 x, Vec3 y, double zLength) : o(o), x(x), y(y) {
    z = (x ^ y).norm(zLength);
}
Ray PerspectiveCamera::apply(const Vec2 &t) const {
    return {o, (x * t.x() - y * t.y() - z).norm()};
}

#endif
#endif /* cameras_perspective_hpp */
