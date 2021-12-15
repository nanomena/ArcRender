#ifndef medium_hpp
#define medium_hpp

class Medium {
public:
    virtual Spectrum evaluate(double t) const {
        throw invalid_argument("NotImplementedError");
    }
    virtual shared_ptr<Shape> sample(double &t, Sampler &RNG) const {
        throw invalid_argument("NotImplementedError");
    }

};

#ifdef ARC_IMPLEMENTATION


#endif
#endif //medium_hpp
