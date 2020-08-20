#ifndef shapes_disc_hpp
#define shapes_disc_hpp

#include "../shape.hpp"

class Disc : public Shape
{
    Vec3 o, T_o, norm;
    double r;
    Mat3 T;

public:
    Disc(Vec3 _o, Vec3 _norm, double _r);

    void inter(const Ray &ray, int &is_inter, Vec3 &intersect) const override;
    Vec3 normal(const Vec3 &inter) const override;
};

#ifdef ARC_IMPLEMENTATION

Disc::Disc(Vec3 _o, Vec3 _norm, double _r)
{
    o = _o;
    norm = _norm.scale(1);
    r = _r;
    Vec3 x, y, z = norm;

    if (z.d[0] * z.d[0] + z.d[1] * z.d[1] > EPS)
        x = Vec3(z.d[1], -z.d[0], 0).scale(1);
    else
        x = Vec3(1, 0, 0);
    y = (x ^ z).scale(1);

    T = axis(x, y, z);
    T_o = T * o;

    Vec3 p(
        (Vec3(1, 0, 0) ^ norm).norm(),
        (Vec3(0, 1, 0) ^ norm).norm(),
        (Vec3(0, 0, 1) ^ norm).norm()
    );
    box = Cuboid(o - p * r, o + p * r);
}

void Disc::inter(const Ray &ray, int &is_inter, Vec3 &intersect) const
{
    Ray T_ray = Ray(T * ray.o, T * ray.d);
    if (abs(T_ray.d.d[2]) < EPS) return (is_inter = 0, void());

    double dis = (T_o.d[2] - T_ray.o.d[2]) / T_ray.d.d[2];
    if (dis < 0) return (is_inter = 0, void());
    Vec3 T_candi = T_ray.o + T_ray.d * dis;

    int inside = ((T_candi - T_o).norm2() < r * r);

    if (!inside) return (is_inter = 0, void());
    is_inter = 1;
    intersect = ray.o + ray.d * dis;
}
Vec3 Disc::normal(const Vec3 &inter) const
{
    return norm;
}

#endif
#endif /* shapes_disc_hpp */
