#ifndef camera_hpp
#define camera_hpp

#include "utils.hpp"
#include "geometry.hpp"

class Camera
{

public:
    Ray virtual apply(const Vec2 &t);
};

class PerspectiveCamera : public Camera
{
    Vec3 o, x, y, z;

public:
    PerspectiveCamera (Vec3 _o, Vec3 _x, Vec3 _y, double _z);
    Ray apply(const Vec2 &t) override;
};

#ifdef ARC_IMPLEMENTATION

Ray Camera::apply(const Vec2 &t)
{
    throw "NotImplementedError";
}
PerspectiveCamera::PerspectiveCamera (Vec3 _o, Vec3 _x, Vec3 _y, double _z)
{
    o = _o; x = _x; y = _y;
    z = (x ^ y); z = z / z.norm() * _z;
}
Ray PerspectiveCamera::apply(const Vec2 &t)
{
    return Ray(o, (x * t.d[0] - y * t.d[1] - z).scale(1));
}

#endif
#endif /* camera_hpp */
