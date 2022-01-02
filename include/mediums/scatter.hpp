#ifndef mediums_scatter_hpp
#define mediums_scatter_hpp

#include "../medium.hpp"

class Scatter : public Medium {
public:
    explicit Scatter(double lambda, Spectrum color);

    Spectrum evaluate(double t) const override;
    Spectrum sample(
        const shared_ptr<Scene> &scene, const Ray &v, shared_ptr<Object> &object, Vec3 &intersect, Sampler &RNG
    ) const override;

private:
    double lambda;
    shared_ptr<Particle> particle;
};

#ifdef ARC_IMPLEMENTATION

Scatter::Scatter(double lambda, Spectrum color) : lambda(lambda), particle(make_shared<Particle>(color)) {}

Spectrum Scatter::evaluate(double t) const {
    return Spectrum(1) * exp(-lambda * t);
}

Spectrum Scatter::sample(
    const shared_ptr<Scene> &scene, const Ray &v, shared_ptr<Object> &object, Vec3 &intersect, Sampler &RNG
) const {
    double t;
    shared_ptr<Shape> shape;
    scene->intersect(v, shape, t);
    double tCur = log(1 - RNG.rand()) / (-lambda);
    if (tCur > t) {
        object = shape;
        intersect = v.o + v.d * t;
    } else {
        object = particle;
        intersect = v.o + v.d * tCur;
    }
    return Spectrum(1);
}



#endif
#endif //mediums_scatter_hpp
