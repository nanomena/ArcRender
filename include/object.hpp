#ifndef object_hpp
#define object_hpp

#include "utils.hpp"
#include "bxdf.hpp"
#include "shape.hpp"
#include "surface.hpp"

class Object {
    shared_ptr<Shape> shape;
    shared_ptr<Surface> surface;

public:
    Object(shared_ptr<Shape> shape_, shared_ptr<Surface> surface_, string name_);

    string name;

    Box3 outline() const;
    pair<int, int> surface_info() const;
    bool intersect(const Ray &ray, double &t) const;
    void evaluate_VtS(const Ray &V, Spectrum &spectrum);
    void evaluate_StV(const Ray &Vb, Spectrum &spectrum);
    void evaluate_VtL(const Ray &V, const Ray &L, Spectrum &spectrum);
    void evaluate_LtV(const Ray &Lb, const Ray &Vb, Spectrum &spectrum);
    void sample_VtL(const Ray &V, Ray &L, double &pdf);
    void sample_LtV(const Ray &Lb, Ray &Vb, double &pdf);
    void sample_S(const Vec3 &ref, Ray &V, double &pdf);
    void sample_StV(Ray &Vb, double &pdf);
};

#ifdef ARC_IMPLEMENTATION

Object::Object(shared_ptr<Shape> shape_, shared_ptr<Surface> surface_, string name_)
    : shape(std::move(shape_)), surface(std::move(surface_)), name(std::move(name_)) {}

Box3 Object::outline() const {
    return shape->outline();
}

pair<int, int> Object::surface_info() const {
    return surface->surface_info();
}

bool Object::intersect(const Ray &ray, double &t) const {
    bool ok = shape->intersect(ray, t);
    if (t < EPS) return false;
    return ok;
}

void Object::evaluate_VtS(const Ray &V, Spectrum &spectrum) {
    double t;
    bool f = shape->intersect(V, t);
    assert(f);
    Vec3 intersect = V.o + V.d * t;
    Vec3 N = shape->normal(intersect);
    surface->evaluate_VtS(intersect, N, -V.d.norm(), spectrum);
}

void Object::evaluate_StV(const Ray &Vb, Spectrum &spectrum) {
    Vec3 intersect = Vb.o;
    Vec3 N = shape->normal(intersect);
    surface->evaluate_StV(intersect, N, Vb.d.norm(), spectrum);
}

void Object::evaluate_VtL(const Ray &V, const Ray &L, Spectrum &spectrum) {
    Vec3 intersect = L.o;
    Vec3 N = shape->normal(intersect);
//    cerr << "evaluate VtL" << " " << V.o << " " << V.d << " " << L.o << " " << L.d << endl;
    surface->evaluate_VtL(intersect, N, -V.d.norm(), L.d.norm(), spectrum);
}

void Object::evaluate_LtV(const Ray &Lb, const Ray &Vb, Spectrum &spectrum) {
    Vec3 intersect = Vb.o;
    Vec3 N = shape->normal(intersect);
    surface->evaluate_LtV(intersect, N, -Lb.d.norm(), Vb.d.norm(), spectrum);
}

void Object::sample_VtL(const Ray &V, Ray &L, double &pdf) {
    double t;
    bool f = shape->intersect(V, t);
    assert(f);
    Vec3 intersect = V.o + V.d * t;
    L.o = intersect;
    Vec3 N = shape->normal(intersect);
    surface->sample_VtL(intersect, N, -V.d.norm(), L.d, pdf);
}

void Object::sample_LtV(const Ray &Lb, Ray &Vb, double &pdf) {
    double t;
    bool f = shape->intersect(Lb, t);
    assert(f);
    Vec3 intersect = Lb.o + Lb.d * t;
    Vb.o = intersect;
    Vec3 N = shape->normal(intersect);
    surface->sample_LtV(intersect, N, -Lb.d.norm(), Vb.d, pdf);
}

void Object::sample_S(const Vec3 &ref, Ray &V, double &pdf) {
    Vec3 pos;
    shape->sample(pos, pdf);
    V = Ray(ref, pos - ref);
    Vec3 N = shape->normal(pos);
    pdf = pdf / (abs(V.d * N) / pow(V.d.length(), 3));
}

void Object::sample_StV(Ray &Vb, double &pdf) {
    double pdf1, pdf2;
    shape->sample(Vb.o, pdf1);
    Vec3 N = shape->normal(Vb.o);
    surface->sample_StV(Vb.o, N, Vb.d, pdf2);
    pdf = pdf1 * pdf2;
}

#endif
#endif /* object_hpp */
