#ifndef medium_hpp
#define medium_hpp

#include "object.hpp"

class Object;
class Scene;

class Medium {
public:
    virtual Spectrum evaluate(double t) const {
        throw invalid_argument("NotImplementedError");
    }
    virtual Spectrum sample(
        const shared_ptr<Scene> &scene, const Ray &v, shared_ptr<Object> &object, Vec3 &intersect, Sampler &RNG
    ) const {
        throw invalid_argument("NotImplementedError");
    }
};

#ifdef ARC_IMPLEMENTATION

#endif
#endif //medium_hpp
