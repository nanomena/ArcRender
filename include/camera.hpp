#ifndef camera_hpp
#define camera_hpp

#include "utils.hpp"
#include "vecmath.hpp"

class Camera {
public:
    virtual Ray evaluate(const Vec3 &pos, Vec2 &t) const {
        throw invalid_argument("NotImplementedError");
    }
    virtual Ray sample(const Vec2 &t, double &pdf, Sampler &RNG) const {
        throw invalid_argument("NotImplementedError");
    }
    virtual double evaluateImportance(const Vec3 &pos) const {
        throw invalid_argument("NotImplementedError");
    }
};

#endif /* camera_hpp */
