#ifndef shapes_sphere_hpp
#define shapes_sphere_hpp

#include "../shape.hpp"

class Sphere : public Shape {
    Vec3 o;
    double r;

    bool contain(const Vec3 &t) const;

public:
    Sphere(Vec3 o, double r);

    bool intersect(const Ray &ray, double &t) const override;
    Vec3 normal(const Vec3 &inter) const override;
};

#ifdef ARC_IMPLEMENTATION

Sphere::Sphere(Vec3 o, double r) : o(o), r(r) {
    box = Box3(o - Vec3(r, r, r), o + Vec3(r, r, r));
}

bool Sphere::contain(const Vec3 &t) const {
    return (o - t).squaredLength() <= r * r + r * EPS;
}

bool Sphere::intersect(const Ray &ray, double &t) const {
    Vec3 x = (o - ray.o).proj(ray.d), y = (o - ray.o) - x;
    double d2 = r * r - y.squaredLength();
    if (d2 < 0) return false;
    Vec3 chord = ray.d.norm(sqrt(d2));
    if (contain(ray.o)) {
        t = (x + chord).length();
        return true;
    } else {
        if ((x - chord) * ray.d <= 0) return false;
        t = (x - chord).length();
        return true;
    }
}
Vec3 Sphere::normal(const Vec3 &inter) const {
    return (inter - o).norm();
}

#endif
#endif /* shapes_sphere_hpp */