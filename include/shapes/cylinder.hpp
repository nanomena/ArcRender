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

    bool intersect(const Ray &ray, double &t) const override;
    Vec3 normal(const Vec3 &pos) const override;

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

bool Cylinder::intersect(const Ray &ray, double &t) const {
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
    return ok;
}

Vec3 Cylinder::normal(const Vec3 &pos) const {
    Vec3 TPos = T * (pos - v.o);
    Vec3 TNormal(TPos.x(), TPos.y(), 0);
    return TInv * TNormal.norm();
}

#endif
#endif /* shapes_cylinder_hpp */
