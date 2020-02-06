#ifndef CAMERA
#define CAMERA
#include <vector>
#include <memory>
using namespace std;
#include "geo.hpp"

class Camera
{

public:
    Camera () {}
    Ray virtual apply(double dx, double dy) { throw "NotImplementedError"; }
};

class PerspectiveCamera : public Camera
{
    Vec3 o, x, y, z;

public:
    PerspectiveCamera (Vec3 _o, Vec3 _x, Vec3 _y, double _z) : Camera()
    {
        o = _o, x = _x, y = _y;
        z = (x ^ y), z = z / z.norm() * _z;
    }
    Ray apply(double dx, double dy)
    {
        return Ray(o, (x * dx - y * dy + z).scale(1));
    }
};


#endif
