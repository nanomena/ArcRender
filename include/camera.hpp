#ifndef camera_hpp
#define camera_hpp

#include "utils.hpp"
#include "vecmath.hpp"
#include "sampler.hpp"

class Camera {
public:
    virtual double evaluate(const Vec3 &pos, Ray &ray, Vec2 &t, Sampler &RNG) const = 0;
    virtual void sample(Ray &ray, const Vec2 &t, Sampler &RNG) const = 0;
};

#endif /* camera_hpp */
