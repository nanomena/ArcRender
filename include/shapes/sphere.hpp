#ifndef shapes_sphere_hpp
#define shapes_sphere_hpp

#include "../shape.hpp"

class Sphere : public Shape {
public:
    Sphere(
        const BxDF *bxdf, const Light *light,
        const Medium *inside, const Medium *outside,
        Vec3 o, double r, bool reverse = false
    );

    bool intersect(const Ray &ray, double &t) const override;
    Vec3 normal(const Vec3 &pos) const override;

private:

    Vec3 o;
    double r;
    bool reverse;
};

#ifdef ARC_IMPLEMENTATION

Sphere::Sphere(
    const BxDF *bxdf, const Light *light,
    const Medium *inside, const Medium *outside,
    Vec3 o, double r, bool reverse
) : Shape(bxdf, light, inside, outside), o(o), r(r), reverse(reverse) {
    box = Box3(o - Vec3(r, r, r), o + Vec3(r, r, r));
}

bool Sphere::intersect(const Ray &ray, double &t) const {
//    cerr << ray.o << " " << ray.d << " " << o << " " << r << endl;
    double x = (o - ray.o) * ray.d, y = (ray.o + ray.d * x - o).length();
    if (r < y) {
        return false;
    }
    if ((o - ray.o).length() < r + EPS) {
        t = x + sqrt(r * r - y * y);
    } else {
        t = x - sqrt(r * r - y * y);
    }
//    cerr << t << endl;
    if (t < EPS) return false;
    return true;
}

Vec3 Sphere::normal(const Vec3 &pos) const {
    return (pos - o).norm() * (reverse ? -1 : 1);
}

#endif
#endif /* shapes_sphere_hpp */