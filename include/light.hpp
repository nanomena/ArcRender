#ifndef Light_hpp
#define Light_hpp

#include "utils.hpp"
#include "sampler.hpp"
#include "surface.hpp"

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

#ifdef ARC_IMPLEMENTATION

#endif
#endif /* Light_hpp */
