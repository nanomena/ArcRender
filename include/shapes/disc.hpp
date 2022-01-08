#ifndef shapes_disc_hpp
#define shapes_disc_hpp

#include "../shape.hpp"

class Disc : public Shape {
public:
    Disc(
        const BxDF *bxdf, const Light *light,
        const Medium *inside, const Medium *outside,
        Ray v, double r
    );

    bool intersect(const Ray &ray, double &t, Vec3 &pos, Vec2 &texPos) const override;
    Vec3 normal(const Vec2 &texPos) const override;
    void sampleSurface(Vec3 &pos, Vec2 &texPos, double &pdf, Sampler &RNG) const override;
    double evaluateSurfaceImportance(const Vec3 &pos, const Vec2 &texPos) const override;

private:

    Mat3 T, TInv;
    Ray v; double r;
};

#ifdef ARC_IMPLEMENTATION

Disc::Disc(
    const BxDF *bxdf, const Light *light,
    const Medium *inside, const Medium *outside,
    Ray v, double r
) : Shape(bxdf, light, inside, outside), v(v), r(r) {
    box = discBox(v, {r, 0});
    rotateAxis(v.d, v.d, T, TInv);
}

bool Disc::intersect(const Ray &ray, double &t, Vec3 &pos, Vec2 &texPos) const {
    if (abs((T * ray.d).z()) < EPS) return false;

    Vec3 To = T * (v.o - ray.o), Td = T * ray.d;
    t = To.z() / Td.z();
    if ((To - Td * t).xy().length() > r) return false;
    if (t < EPS) return false;

    pos = ray.o + ray.d * t;
    texPos = (To - Td * t).xy() / r;

    return true;
}

Vec3 Disc::normal(const Vec2 &texPos) const {
    return v.d;
}

void Disc::sampleSurface(Vec3 &pos, Vec2 &texPos, double &pdf, Sampler &RNG) const {
    double theta = RNG.rand(0, 2 * pi), x = RNG.rand(0, r * r);
    Vec3 TPos = {sin(theta) * sqrt(x), cos(theta) * sqrt(x), 0};
    pos = TInv * TPos + v.o;
    texPos = TPos.xy() / r;
    pdf = 1. / (pi * r * r);
}

double Disc::evaluateSurfaceImportance(const Vec3 &pos, const Vec2 &texPos) const {
    return 1. / (pi * r * r);
}
#endif
#endif /* shapes_disc_hpp */
