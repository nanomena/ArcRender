#ifndef medium_hpp
#define medium_hpp

#include "shape.hpp"

class Shape;

class Medium {
public:
    virtual Spectrum evaluate(double t) const {
        throw invalid_argument("NotImplementedError");
    }
    virtual Spectrum sample(double &t, shared_ptr<Shape> &object, Sampler &RNG) const {
        throw invalid_argument("NotImplementedError");
    }

};

#ifdef ARC_IMPLEMENTATION


#endif
#endif //medium_hpp
