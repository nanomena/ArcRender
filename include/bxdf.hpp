#ifndef bxdf_hpp
#define bxdf_hpp

#include "utils.hpp"
#include "sampler.hpp"

class BxDF
{

public:
    virtual void evaluate(const Vec3 &V, const Vec3 &L, double &weight)
    {
        throw invalid_argument("NotImplementedError");
    }
    virtual void sample_VtL(const Vec3 &V, Vec3 &L, double &pdf)
    {
        throw invalid_argument("NotImplementedError");
    }
    virtual void sample_LtV(const Vec3 &L, Vec3 &V, double &pdf)
    {
        throw invalid_argument("NotImplementedError");
    }
    void verify_sample();
};

#ifdef ARC_IMPLEMENTATION

void BxDF::verify_sample()
{
    const int cnt = 1000000;
    for (int i = 0; i < 90; i += 10)
    {
        double w = i * pi / 180;
        Vec3 V(sin(w), 0, cos(w));
        double sum = 0;
        for (int j = 0; j < cnt; ++ j)
        {
            Vec3 L; double pdf;
            sample_VtL(V, L, pdf);
            sum += 1 / pdf;
        }
        cout << i << ": " << sum / cnt << endl;
    }
}

#endif
#endif /* bxdf_hpp */
