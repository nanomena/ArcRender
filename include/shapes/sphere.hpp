#ifndef shapes_sphere_hpp
#define shapes_sphere_hpp

#include "../shape.hpp"

class Sphere : public Shape
{
    Vec3 o;
    double r;

    bool contain(const Vec3 &t) const;

public:
    Sphere(Vec3 _o, double _r);

    void inter(const Ray &ray, int &is_inter, Vec3 &intersect) const override;
    Vec3 normal(const Vec3 &inter) const override;
};

#ifdef ARC_IMPLEMENTATION

Sphere::Sphere(Vec3 _o, double _r)
{
    o = _o;
    r = _r;
    box = Cuboid(o - Vec3(r, r, r), o + Vec3(r, r, r));
}

bool Sphere::contain(const Vec3 &t) const
{
    return (o - t).norm2() <= r * r + r * EPS;
}

void Sphere::inter(const Ray &ray, int &is_inter, Vec3 &intersect) const
{
    Vec3 x = (o - ray.o).project(ray.d), y = (o - ray.o) - x;
    double d2 = r * r - y.norm2();
    if (d2 < 0)
    {
        is_inter = 0;
        return;
    }
    Vec3 chord = ray.d.scale(sqrt(d2));
    if (contain(ray.o))
    {
        is_inter = 1;
        intersect = ray.o + x + chord;
    }
    else
    {
        Vec3 candi = ray.o + x - chord;
        if ((candi - ray.o) * ray.d <= 0)
        {
            is_inter = 0;
            return;
        }
        is_inter = 1;
        intersect = candi;
    }
}
Vec3 Sphere::normal(const Vec3 &inter) const
{
    return (inter - o).scale(1);
}

#endif
#endif /* shapes_sphere_hpp */