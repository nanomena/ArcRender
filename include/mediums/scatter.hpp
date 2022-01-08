#ifndef mediums_scatter_hpp
#define mediums_scatter_hpp

#include "../medium.hpp"

class Scatter : public Medium {
public:
    explicit Scatter(double lambda, Spectrum color);
    ~Scatter();

    Spectrum evaluate(double t) const override;
    Spectrum sample(
        const Scene *scene, const Ray &v, const Object *&object, Vec3 &pos, Vec2 &texPos, Sampler &RNG
    ) const override;

private:
    double lambda;
    const Particle *particle;
};

#ifdef ARC_IMPLEMENTATION

Scatter::Scatter(double lambda, Spectrum color) : lambda(lambda), particle(new Particle(color)) {}
Scatter::~Scatter() {delete particle;}

Spectrum Scatter::evaluate(double t) const {
    return Spectrum(1) * exp(-lambda * t);
}

Spectrum Scatter::sample(
    const Scene *scene, const Ray &v, const Object *&object, Vec3 &pos, Vec2 &texPos, Sampler &RNG
) const {
    double t;
    const Shape *shape;
    scene->intersect(v, shape, t, pos, texPos);
    double tCur = log(1 - RNG.rand()) / (-lambda);
    if (tCur > t) {
        object = shape;
    } else {
        object = particle;
        pos = v.o + v.d * tCur;
        texPos = Vec2(0, 0);
    }
    return Spectrum(1);
}



#endif
#endif //mediums_scatter_hpp
