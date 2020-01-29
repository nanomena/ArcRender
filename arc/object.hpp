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

    Vec3 inter(const Photon &photon) const
    {
        return shape->inter(photon.ray);
    }
    Photon forward(const Photon &photon, const Vec3 &inter) const
    {
        Ray normal = shape->normal(photon.ray, inter);
        $ << "hit " << name << " normal " << normal << endl;
        Photon result = surface->forward(photon, normal);
        $ << "tracing" << result.ray << endl;
        return result;
    }
};


#endif
