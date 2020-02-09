#ifndef object_hpp
#define object_hpp

#include "utils.hpp"
#include "surface.hpp"
#include "shape.hpp"

class Object
{
    shared_ptr<Shape> shape;
    shared_ptr<Surface> surface;

public:
    string name;
    Object (shared_ptr<Shape> _shape, shared_ptr<Surface> _surface, string _name);

    Cuboid outline() const;
    void inter(const Ray &ray, int &hit, Vec3 &hitpoint) const;
    double forward(const Vec3 &inter, Photon &photon, int &type) const;
};

#ifdef library


Object::Object (shared_ptr<Shape> _shape, shared_ptr<Surface> _surface, std::string _name)
{
    shape = _shape;
    surface = _surface;
    name = _name;
}

Cuboid Object::outline() const
{
    return shape->outline();
}
void Object::inter(const Ray &ray, int &hit, Vec3 &hitpoint) const
{
    shape->inter(ray, hit, hitpoint);
    if ((hitpoint - ray.o).norm2() < EPS) hit = 0;
}
double Object::forward(const Vec3 &inter, Photon &photon, int &type) const
{
    $ << name << endl;
    Ray normal = shape->normal(photon.ray, inter);

    Vec3 x, y, z = normal.d;
    if ((photon.ray.d ^ normal.d).norm2() > EPS)
        x = (photon.ray.d - z * (photon.ray.d * z)).scale(1);
    else if (z.d[0] * z.d[0] + z.d[1] * z.d[1] > EPS)
        x = Vec3(z.d[1], -z.d[0], 0).scale(1);
    else
        x = Vec3(1, 0, 0);
    y = (x ^ z).scale(1);
    Mat3 T = axis_I(x, y, z);

    Vec3 d(x * photon.ray.d, y * photon.ray.d, z * photon.ray.d);
    photon.apply(Ray(Vec3(), d));

    // change into BRDF coordinate

    double weight = surface->forward(photon, type);

    photon.apply(Ray(normal.o, T * photon.ray.d));
    return weight;
}

#endif
#endif /* object_hpp */
