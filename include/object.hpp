#ifndef object_hpp
#define object_hpp

#include <utility>

#include "utils.hpp"
#include "bxdf.hpp"
#include "shape.hpp"
#include "surface.hpp"

class Object
{
    shared_ptr<Shape> shape;
    shared_ptr<Surface> surface;

public:
    Object(shared_ptr<Shape> shape_, shared_ptr<Surface> surface_, string name_);

    string name;

    Cuboid outline() const;
    void inter(const Ray &ray, int &hit, Vec3 &hitpoint) const;
    void evaluate_VtS(const Ray &V, Spectrum &spectrum);
    void evaluate_VtL(const Ray &V, const Ray &L, Spectrum &spectrum);
    void sample_VtL(const Ray &V, Ray &L, double &pdf);
};

#ifdef ARC_IMPLEMENTATION


Object::Object(shared_ptr<Shape> shape_, shared_ptr<Surface> surface_, string name_)
    : shape(std::move(shape_)), surface(std::move(surface_)), name(std::move(name_)) {}

Cuboid Object::outline() const
{
    return shape->outline();
}

void Object::inter(const Ray &ray, int &hit, Vec3 &hitpoint) const
{
    shape->inter(ray, hit, hitpoint);
    if ((hitpoint - ray.o).norm2() < EPS) hit = 0;
}

void Object::evaluate_VtS(const Ray &V, Spectrum &spectrum)
{
    int hit; Vec3 hitpoint;
    shape->inter(V, hit, hitpoint);
    assert(hit);
    Vec3 N = shape->normal(hitpoint);
    surface->evaluate_VtS(hitpoint, N, -V.d, spectrum);
}

void Object::evaluate_VtL(const Ray &V, const Ray &L, Spectrum &spectrum)
{
    Vec3 hitpoint = L.o;
    Vec3 N = shape->normal(hitpoint);
    surface->evaluate_VtL(hitpoint, N, -V.d, L.d, spectrum);
}

void Object::sample_VtL(const Ray &V, Ray &L, double &pdf)
{
    int hit; Vec3 hitpoint;
    shape->inter(V, hit, hitpoint);
    assert(hit);
    L.o = hitpoint;
    Vec3 N = shape->normal(hitpoint);
    surface->sample_VtL(hitpoint, N, -V.d, L.d, pdf);
}

#endif
#endif /* object_hpp */
