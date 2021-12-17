#ifndef mediums_scatter_hpp
#define mediums_scatter_hpp

#include "../medium.hpp"

class Scatter : public Medium {
public:
    explicit Scatter(double lambda, Spectrum color);

    Spectrum evaluate(double t) const override;
    Spectrum sample(
        const shared_ptr<Scene> &scene, const Ray &v, shared_ptr<Shape> &object, Vec3 &intersect, Sampler &RNG
    ) const override;

private:
    double lambda;
    Spectrum color;
    int maxTrace;
};

#ifdef ARC_IMPLEMENTATION

Scatter::Scatter(double lambda, Spectrum color) : lambda(lambda), color(color) {}

Spectrum Scatter::evaluate(double t) const {
    return Spectrum(1) * exp(-lambda * t);
}

Spectrum Scatter::sample(
    const shared_ptr<Scene> &scene, const Ray &v, shared_ptr<Shape> &object, Vec3 &intersect, Sampler &RNG
) const {
    double t;
    scene->intersect(v, object, t);
    double tCur = log(1 - RNG.rand()) / (-lambda);
    if (tCur > t) {
        intersect = v.o + v.d * t;
        return Spectrum(1);
    } else {
        throw invalid_argument("NotImplementedError");
    }
}



#endif
#endif //mediums_scatter_hpp
