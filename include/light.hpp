#ifndef Light_hpp
#define Light_hpp

#include <utility>

#include "utils.hpp"
#include "sampler.hpp"
#include "surface.hpp"
#include "photon.hpp"

class Light
{

public:
    virtual void evaluate(const Vec3 &V, double &weight)
    {
        throw invalid_argument("NotImplementedError");
    }
    virtual void sample(Vec3 &V, double &pdf)
    {
        throw invalid_argument("NotImplementedError");
    }
};

class UniformLight : public Light
{

public:
    UniformLight() = default;
    void evaluate(const Vec3 &V, double &weight) override;
    void sample(Vec3 &V, double &pdf) override;
};

#ifdef ARC_IMPLEMENTATION

void UniformLight::evaluate(const Vec3 &V, double &weight)
{
    if (V.d[2] < 0)
        return (weight = 0, void());
    weight = 1;
}

void UniformLight::sample(Vec3 &V, double &pdf)
{
    V = RD.semisphere();
    pdf = 1 / (2 * pi);
}

#endif
#endif /* Light_hpp */
