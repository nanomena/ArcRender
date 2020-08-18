#ifndef surface_hpp
#define surface_hpp

#include <utility>

#include "bxdf.hpp"
#include "utils.hpp"
#include "light.hpp"
#include "mapping.hpp"
#include "material.hpp"
#include "spectrum.hpp"

class Surface
{
    vector<pair<shared_ptr<BxDF>, Spectrum>> BxDFs;
    vector<pair<shared_ptr<Light>, Spectrum>> Lights;
//    shared_ptr<Material> inside, outside; @TODO Material
    void rotate_axis(const Vec3 &N, const Vec3 &V, Mat3 &T, Mat3 &T_I) const;

public:
    Surface() {}
    Surface(vector<pair<shared_ptr<BxDF>, Spectrum>> BxDFs_, vector<pair<shared_ptr<Light>, Spectrum>> Lights_);

    pair<int, int> surface_info() const;
    void evaluate_VtS(const Vec3 &inter, const Vec3 &N, const Vec3 &V, Spectrum &spectrum);
    void evaluate_VtL(const Vec3 &inter, const Vec3 &N, const Vec3 &V, const Vec3 &L, Spectrum &spectrum);
    void sample_VtL(const Vec3 &inter, const Vec3 &N, const Vec3 &V, Vec3 &L, double &pdf);
}; // @TODO better interface

shared_ptr<Surface> make_light(shared_ptr<Light> light, Spectrum spectrum);
shared_ptr<Surface> make_bxdf(shared_ptr<BxDF> bxdf, Spectrum spectrum);

#ifdef ARC_IMPLEMENTATION

Surface::Surface(vector<pair<shared_ptr<BxDF>, Spectrum>> BxDFs_, vector<pair<shared_ptr<Light>, Spectrum>> Lights_)
    : BxDFs(BxDFs_), Lights(Lights_) {}

shared_ptr<Surface> make_light(shared_ptr<Light> light, Spectrum spectrum)
{
    vector<pair<shared_ptr<BxDF>, Spectrum>> BxDFs;
    vector<pair<shared_ptr<Light>, Spectrum>> Lights;
    Lights.push_back(make_pair(light, spectrum));
    return make_shared<Surface>(BxDFs, Lights);
}

shared_ptr<Surface> make_bxdf(shared_ptr<BxDF> bxdf, Spectrum spectrum)
{
    vector<pair<shared_ptr<BxDF>, Spectrum>> BxDFs;
    vector<pair<shared_ptr<Light>, Spectrum>> Lights;
    BxDFs.push_back(make_pair(bxdf, spectrum));
    return make_shared<Surface>(BxDFs, Lights);
}

pair<int, int> Surface::surface_info() const
{
    return make_pair(BxDFs.size(), Lights.size());
}

void Surface::rotate_axis(const Vec3 &N, const Vec3 &V, Mat3 &T, Mat3 &T_I) const
{
    assert(abs(1 - N.norm()) < EPS);
    Vec3 x, y, z = N;
    if ((V ^ N).norm2() > EPS)
        x = (V - z * (V * z)).scale(1);
    else if (z.d[0] * z.d[0] + z.d[1] * z.d[1] > EPS)
        x = Vec3(z.d[1], -z.d[0], 0).scale(1);
    else
        x = Vec3(1, 0, 0);
    y = (x ^ z).scale(1);

    T_I = axis_I(x, y, z);
    T = T_I.I();
}

void Surface::evaluate_VtS(const Vec3 &inter, const Vec3 &N, const Vec3 &V, Spectrum &spectrum)
{
    Mat3 T, T_I;
    Vec3 V_, L_;
    rotate_axis(N, V, T, T_I);
    V_ = T * V;
    spectrum = Spectrum();
    for (auto Func : Lights)
    {
        double weight;
        get<0>(Func)->evaluate(V_, weight);
        spectrum = spectrum + get<1>(Func) * Spectrum(weight);
    }
}

void Surface::evaluate_VtL(const Vec3 &inter, const Vec3 &N, const Vec3 &V, const Vec3 &L, Spectrum &spectrum)
{
    Mat3 T, T_I;
    Vec3 V_, L_;
    rotate_axis(N, V, T, T_I);
    V_ = T * V;
    L_ = T * L;
    spectrum = Spectrum();
    for (auto Func : BxDFs)
    {
        double weight;
        get<0>(Func)->evaluate(V_, L_, weight);
        spectrum = spectrum + get<1>(Func) * Spectrum(weight);
    }
}

void Surface::sample_VtL(const Vec3 &inter, const Vec3 &N, const Vec3 &V, Vec3 &L, double &pdf) // @TODO MIS support
{
    Mat3 T, T_I;
    Vec3 V_, L_;
    rotate_axis(N, V, T, T_I);
    V_ = T * V;
    if (BxDFs.size() == 0) { L = T_I * Vec3(0, 0, 1); pdf = 1; return;}
    get<0>(BxDFs[0])->sample(V_, L_, pdf);
    L = T_I * L_;
}

#endif
#endif /* surface_hpp */
