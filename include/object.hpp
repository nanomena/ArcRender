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
    void inter(const Ray &ray, int &is_inter, Vec3 &intersect) const;
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

void Object::inter(const Ray &ray, int &is_inter, Vec3 &intersect) const
{
    shape->inter(ray, is_inter, intersect);
    if ((intersect - ray.o).norm2() < EPS) is_inter = 0;
}

void Object::evaluate_VtS(const Ray &V, Spectrum &spectrum)
{
    int is_inter; Vec3 intersect;
    shape->inter(V, is_inter, intersect);
    assert(is_inter);
    Vec3 N = shape->normal(intersect);
    surface->evaluate_VtS(intersect, N, -V.d, spectrum);
}

void Object::evaluate_VtL(const Ray &V, const Ray &L, Spectrum &spectrum)
{
    Vec3 intersect = L.o;
    Vec3 N = shape->normal(intersect);
    surface->evaluate_VtL(intersect, N, -V.d, L.d, spectrum);
}

void Object::sample_VtL(const Ray &V, Ray &L, double &pdf)
{
    int is_inter; Vec3 intersect;
    shape->inter(V, is_inter, intersect);
    assert(is_inter);
    L.o = intersect;
    Vec3 N = shape->normal(intersect);
    surface->sample_VtL(intersect, N, -V.d, L.d, pdf);
}

#endif
#endif /* object_hpp */
