#ifndef shapes_sphere_hpp
#define shapes_sphere_hpp

#include "../shape.hpp"

class Sphere : public Shape {
public:
    Sphere(
        const shared_ptr<BxDF> &BxDF, const shared_ptr<Light> &Light,
        const shared_ptr<Medium> &inside, const shared_ptr<Medium> &outside,
        Vec3 o, double r, bool reverse = false
    );

    bool intersect(const Ray &ray, double &t) const override;
    Vec3 normal(const Vec3 &inter) const override;

private:

    Vec3 o;
    double r;
    bool reverse;
};

#ifdef ARC_IMPLEMENTATION

Sphere::Sphere(
    const shared_ptr<BxDF> &BxDF, const shared_ptr<Light> &Light,
    const shared_ptr<Medium> &inside, const shared_ptr<Medium> &outside,
    Vec3 o, double r, bool reverse
) : Shape(BxDF, Light, inside, outside), o(o), r(r), reverse(reverse) {
    box = Box3(o - Vec3(r, r, r), o + Vec3(r, r, r));
}

bool Sphere::intersect(const Ray &ray, double &t) const {
//    cerr << ray.o << " " << ray.d << " " << o << " " << r << endl;
    double x = (o - ray.o) * ray.d, y = (ray.o + ray.d * x - o).length();
    if (r + EPS < y) {
//        cerr << "Not" << endl;
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

Vec3 Sphere::normal(const Vec3 &inter) const {
    return (inter - o).norm() * (reverse ? -1 : 1);
}

#endif
#endif /* shapes_sphere_hpp */