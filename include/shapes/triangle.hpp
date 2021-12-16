#ifndef shapes_triangle_hpp
#define shapes_triangle_hpp

#include "../shape.hpp"

class Triangle : public Shape {
    Vec3 V0, V1, V2, norm;

public:
    Triangle(
        const shared_ptr<BxDF> &BxDF, const shared_ptr<Light> &Light,
        const shared_ptr<Medium> &inside, const shared_ptr<Medium> &outside,
        Vec3 V0, Vec3 V1, Vec3 V2
    );

    bool intersect(const Ray &ray, double &t) const override;
    Vec3 normal(const Vec3 &inter) const override;
};

#ifdef ARC_IMPLEMENTATION

Triangle::Triangle(
    const shared_ptr<BxDF> &BxDF, const shared_ptr<Light> &Light,
    const shared_ptr<Medium> &inside, const shared_ptr<Medium> &outside,
    Vec3 V0, Vec3 V1, Vec3 V2
) : Shape(BxDF, Light, inside, outside), V0(V0), V1(V1), V2(V2) {
    norm = ((V1 - V0) ^ (V2 - V0)).norm();
    box = Box3(V0) + Box3(V1) + Box3(V2);
}

bool Triangle::intersect(const Ray &ray, double &t) const {
    Vec3 E1 = V1 - V0, E2 = V2 - V0, T = ray.o - V0;
    Vec3 P = (ray.d ^ E2);
    double det = P * E1;
    if (det < 0) det = -det, T = -T;
    if (det < EPS) return false;
    double u = P * T;
    if (u < 0 || u > det) return false;
    Vec3 Q = (T ^ E1);
    t = Q * E2 / det;
    if (t < EPS) return false;
    double v = Q * ray.d;
    if (v < 0 || u + v > det) return false;
    return true;
}

Vec3 Triangle::normal(const Vec3 &inter) const {
    return norm;
}

//Vec3 Triangle::normal(const Vec3 &inter) const
//{
//    double S = ((V1 - V0) ^ (V2 - V0)).norm();
//    double k1 = ((inter - V0) ^ (inter - V2)).norm() / S;
//    double k2 = ((inter - V0) ^ (inter - V1)).norm() / S;
//    return (Vn0 * (1 - k1 - k2) + Vn1 * k1 + Vn2 * k2).scale(1);
//}

#endif
#endif /* shapes_triangle_hpp */
