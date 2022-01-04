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

    bool intersect(const Ray &ray, double &t) const override;
    Vec3 normal(const Vec3 &inter) const override;
    void sampleSurface(Vec3 &pos, double &pdf, Sampler &RNG) const override;
    double evaluateSurfaceImportance(const Vec3 &pos) const override;

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
    box = Box3(v.o - Vec3(r, r, r), v.o + Vec3(r, r, r));
    rotateAxis(v.d, v.d, T, TInv);
}

bool Disc::intersect(const Ray &ray, double &t) const {
    if (abs((T * ray.d).z()) < EPS) return false;

    t = (T * (v.o - ray.o)).z() / (T * ray.d).z();

    if (t < EPS) return false;
    return true;
}
Vec3 Disc::normal(const Vec3 &inter) const {
    return v.d;
}

void Disc::sampleSurface(Vec3 &pos, double &pdf, Sampler &RNG) const {
    double theta = RNG.rand(0, 2 * pi), x = RNG.rand(0, r * r);
    Vec3 posT = {sin(theta) * sqrt(x), cos(theta) * sqrt(x), 0};
    pos = TInv * posT + v.o;

//#pragma omp critical
//    cerr << (pos - v.o) * v.d << endl;
    pdf = 1. / (pi * r * r);
}

double Disc::evaluateSurfaceImportance(const Vec3 &pos) const {
    return 1. / (pi * r * r);
}
#endif
#endif /* shapes_disc_hpp */
