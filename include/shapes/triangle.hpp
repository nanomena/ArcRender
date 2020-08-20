#ifndef shapes_triangle_hpp
#define shapes_triangle_hpp

#include "../shape.hpp"

class Triangle : public Shape
{
    Vec3 V0, V1, V2, norm;

public:
    Triangle(Vec3 V0_, Vec3 V1_, Vec3 V2_);

    void inter(const Ray &ray, int &is_inter, Vec3 &intersect) const override;
    Vec3 normal(const Vec3 &inter) const override;
};

#ifdef ARC_IMPLEMENTATION


Triangle::Triangle(Vec3 V0_, Vec3 V1_, Vec3 V2_)
{
    V0 = V0_;
    V1 = V1_;
    V2 = V2_;
    norm = ((V1 - V0) ^ (V2 - V0)).scale(1);
    box = Cuboid(V0) + Cuboid(V1) + Cuboid(V2);
}

void Triangle::inter(const Ray &ray, int &is_inter, Vec3 &intersect) const
{
    Vec3 E1 = V1 - V0, E2 = V2 - V0, T = ray.o - V0;
    Vec3 P = (ray.d ^ E2);
    double det = P * E1;
    if (det < 0) det = -det, T = -T;
    if (det < EPS) return (is_inter = 0, void());
    double u = P * T;
    if (u < 0 || u > det) return (is_inter = 0, void());
    Vec3 Q = (T ^ E1);
    double t = Q * E2;
    if (t < 0) return (is_inter = 0, void());
    double v = Q * ray.d;
    if (v < 0 || u + v > det) return (is_inter = 0, void());
    is_inter = 1;
    intersect = ray.o + ray.d * (t / det);
}

Vec3 Triangle::normal(const Vec3 &inter) const
{
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
