#ifndef medium_hpp
#define medium_hpp

#include "shape.hpp"

class Shape;
class Scene;

class Medium {
public:
    virtual Spectrum evaluate(double t) const {
        throw invalid_argument("NotImplementedError");
    }
    virtual Spectrum sample(
        const shared_ptr<Scene> &scene, const Ray &v, shared_ptr<Shape> &object, Vec3 &intersect, Sampler &RNG
    ) const {
        throw invalid_argument("NotImplementedError");
    }

    string identifier;
};

#ifdef ARC_IMPLEMENTATION

#endif
#endif //medium_hpp
