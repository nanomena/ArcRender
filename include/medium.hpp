#ifndef medium_hpp
#define medium_hpp

#include "shape.hpp"

class Shape;
class Scene;

class Medium {
public:
    virtual Spectrum evaluate(
        const shared_ptr<Scene> &scene, const Ray &v, const shared_ptr<Shape> &object, const Vec3 &intersect,
        const shared_ptr<Shape> &tObject, const Vec3 &target, const function<Spectrum(const Vec3 &)> &link, Sampler &RNG
    ) const {
        throw invalid_argument("NotImplementedError");
    }
    virtual Spectrum sample(
        const shared_ptr<Scene> &scene, const Ray &v, Ray &l, shared_ptr<Shape> &object, Vec3 &intersect, Sampler &RNG
    ) const {
        throw invalid_argument("NotImplementedError");
    }

    string identifier;
};

#ifdef ARC_IMPLEMENTATION

#endif
#endif //medium_hpp
