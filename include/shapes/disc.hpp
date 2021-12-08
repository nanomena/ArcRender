#ifndef shapes_disc_hpp
#define shapes_disc_hpp

#include "../shape.hpp"

class Disc : public Shape {
    Vec3 o, T_o, norm;
    double r;
    Mat3 T;

public:
    Disc(Vec3 o, Vec3 norm, double r);

    bool intersect(const Ray &ray, double &t) const override;
    Vec3 normal(const Vec3 &inter) const override;
};

#ifdef ARC_IMPLEMENTATION

Disc::Disc(Vec3 o, Vec3 norm, double r) : o(o), norm(norm.norm()), r(r) {
    Vec3 x, y, z = norm;

    if (z.x() * z.x() + z.y() * z.y() > EPS)
        x = Vec3(z.y(), -z.x(), 0).norm();
    else
        x = Vec3(1, 0, 0);
    y = (x ^ z).norm();

    T = makeAxis(x, y, z);
    T_o = T * o;

    Vec3 p(
        (Vec3(1, 0, 0) ^ norm).length(),
        (Vec3(0, 1, 0) ^ norm).length(),
        (Vec3(0, 0, 1) ^ norm).length()
    );
    box = Box3(o - p * r, o + p * r);
}

bool Disc::intersect(const Ray &ray, double &t) const {
    Ray T_ray = Ray(T * ray.o, T * ray.d);
    if (abs(T_ray.d.z()) < EPS) return false;

    t = (T_o.z() - T_ray.o.z()) / T_ray.d.z();
    if (t < 0) return false;
    Vec3 T_candi = T_ray.o + T_ray.d * t;

    int inside = ((T_candi - T_o).squaredLength() < r * r);
    return inside;
}
Vec3 Disc::normal(const Vec3 &inter) const {
    return norm;
}

#endif
#endif /* shapes_disc_hpp */
