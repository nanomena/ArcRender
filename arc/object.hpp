#ifndef OBJECT
#define OBJECT
#include "geo.hpp"
#include "shape.hpp"
#include "surface.hpp"
#include <memory>
#include <string>
using namespace std;

class Object
{
    shared_ptr<Shape> shape;
    shared_ptr<Surface> surface;
    string name;

public:
    Object (shared_ptr<Shape> _shape, shared_ptr<Surface> _surface, string _name)
    {
        shape = _shape;
        surface = _surface;
        name = _name;
    }

    Vec3 inter(const Ray &ray) const
    {
        return shape->inter(ray);
    }
    double forward(const Vec3 &inter, Photon &photon) const
    {
        Ray normal = shape->normal(photon.ray, inter);
        $ << "hit " << name << " normal " << normal << endl;

        Vec3 x, y, z = normal.d;
        if ((photon.d ^ normal.d).norm2() > EPS)
            x = (photon.d - z * (photon.d * z)).scale(1);
        else if (z.d[0] * z.d[0] + z.d[1] * z.d[1] > EPS)
            x = Vec3(z.d[1], -z.d[0], 0).scale(1);
        else
            x = Vec3(1, 0, 0);
        y = (x ^ z).scale(1);
        Mat3 T = axis_I(x, y, z);

        Vec3 d(x * photon.ray.d, y * photon.ray.d, z * photon.ray.d);
        photon.apply(Ray(Vec3(), d));

        // change into BRDF coordinate

        double weight = surface->forward(photon);

        photon.apply(Ray(normal.o, T * photon.ray.d));

        return weight;
    }
};


#endif
