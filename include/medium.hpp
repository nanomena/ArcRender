#ifndef medium_hpp
#define medium_hpp

#include "object.hpp"

class Object;
class Scene;

class Medium {
public:
    virtual Spectrum evaluate(const Ray &v, double t) const {
        throw invalid_argument("NotImplementedError");
    }
    virtual Spectrum sample(
        const Scene *scene, const Ray &v, const Object *&object, Vec3 &pos, Vec2 &texPos, Sampler &RNG
    ) const {
        throw invalid_argument("NotImplementedError");
    }
};

#ifdef ARC_IMPLEMENTATION

#endif
#endif //medium_hpp
