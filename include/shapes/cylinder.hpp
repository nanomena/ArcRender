#ifndef shapes_cylinder_hpp
#define shapes_cylinder_hpp

#include "../shape.hpp"

class Cylinder : public Shape {
public:
    Cylinder(
        const BxDF *bxdf, const Light *light,
        const Medium *inside, const Medium *outside,
        Ray v, double vx, double vy0, double vy1
    );

    bool intersect(const Ray &ray, double &t, Vec3 &pos, Vec2 &texPos) const override;
    Vec3 normal(const Vec2 &texPos) const override;

private:

    Mat3 T, TInv;
    Ray v; double vx, vy0, vy1;
};

#ifdef ARC_IMPLEMENTATION

Cylinder::Cylinder(
    const BxDF *bxdf, const Light *light,
    const Medium *inside, const Medium *outside,
    Ray v, double vx, double vy0, double vy1
) : Shape(bxdf, light, inside, outside), v(v), vx(vx), vy0(vy0), vy1(vy1) {
    box = discBox(v, {vx, vy0}) + discBox(v, {vx, vy1});
    rotateAxis(v.d, v.d, T, TInv);
}

bool Cylinder::intersect(const Ray &ray, double &t, Vec3 &pos, Vec2 &texPos) const {
    Ray TRay = Ray(T * (ray.o - v.o), T * ray.d);
    if (abs(TRay.d.z()) < EPS) return false;

    bool ok = false;
    double x = -TRay.o.xy() * TRay.d.xy().norm(), y = (TRay.o.xy() + TRay.d.xy().norm() * x).length();
    if (vx < y) return false;

    {
        t = (x - sqrt(vx * vx - y * y)) / TRay.d.xy().length();
        ok = t > EPS && vy0 <= (TRay.o + TRay.d * t).z() && (TRay.o + TRay.d * t).z() <= vy1;
    }
    if (!ok) {
        t = (x + sqrt(vx * vx - y * y)) / TRay.d.xy().length();
        ok = t > EPS && vy0 <= (TRay.o + TRay.d * t).z() && (TRay.o + TRay.d * t).z() <= vy1;
    }
    if (ok) {
        pos = ray.o + ray.d * t;
        Vec3 TPos = TRay.o + TRay.d * t;
        texPos = {atan2(TPos.y(), TPos.x()), (TPos.z() - vy0) / (vy1 - vy0)};
    }

    return ok;
}

Vec3 Cylinder::normal(const Vec2 &texPos) const {
    Vec3 TNormal(cos(texPos.x()), sin(texPos.x()), 0);
    return TInv * TNormal.norm();
}

#endif
#endif /* shapes_cylinder_hpp */
