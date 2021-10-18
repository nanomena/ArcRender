#ifndef surface_hpp
#define surface_hpp

#include "bxdf.hpp"
#include "utils.hpp"
#include "light.hpp"
#include "mapping.hpp"
#include "material.hpp"
#include "spectrum.hpp"

class Surface {
    vector<pair<shared_ptr<BxDF>, Spectrum>> BxDFs;
    vector<pair<shared_ptr<Light>, Spectrum>> Lights;
//    shared_ptr<Material> inside, outside; @TODO Material
    void rotate_axis(const Vec3 &N, const Vec3 &V, Mat3 &T, Mat3 &T_I) const;

public:
    Surface() {}
    Surface(vector<pair<shared_ptr<BxDF>, Spectrum>> BxDFs_, vector<pair<shared_ptr<Light>, Spectrum>> Lights_);

    pair<int, int> surface_info() const;
    void evaluate_VtS(const Vec3 &inter, const Vec3 &N, const Vec3 &V, Spectrum &spectrum);
    void evaluate_StV(const Vec3 &inter, const Vec3 &N, const Vec3 &V, Spectrum &spectrum);
    void evaluate_VtL(const Vec3 &inter, const Vec3 &N, const Vec3 &V, const Vec3 &L, Spectrum &spectrum);
    void evaluate_LtV(const Vec3 &inter, const Vec3 &N, const Vec3 &L, const Vec3 &V, Spectrum &spectrum);
    void sample_VtL(const Vec3 &inter, const Vec3 &N, const Vec3 &V, Vec3 &L, double &pdf);
    void sample_LtV(const Vec3 &inter, const Vec3 &N, const Vec3 &L, Vec3 &V, double &pdf);
    void sample_StV(const Vec3 &inter, const Vec3 &N, Vec3 &Vb, double &pdf);
}; // @TODO better interface

shared_ptr<Surface> make_light(shared_ptr<Light> light, Spectrum spectrum);
shared_ptr<Surface> make_bxdf(shared_ptr<BxDF> bxdf, Spectrum spectrum);

#ifdef ARC_IMPLEMENTATION

Surface::Surface(vector<pair<shared_ptr<BxDF>, Spectrum>> BxDFs_, vector<pair<shared_ptr<Light>, Spectrum>> Lights_)
    : BxDFs(BxDFs_), Lights(Lights_) {}

shared_ptr<Surface> make_light(shared_ptr<Light> light, Spectrum spectrum) {
    vector<pair<shared_ptr<BxDF>, Spectrum>> BxDFs;
    vector<pair<shared_ptr<Light>, Spectrum>> Lights;
    Lights.push_back(make_pair(light, spectrum));
    return make_shared<Surface>(BxDFs, Lights);
}

shared_ptr<Surface> make_bxdf(shared_ptr<BxDF> bxdf, Spectrum spectrum) {
    vector<pair<shared_ptr<BxDF>, Spectrum>> BxDFs;
    vector<pair<shared_ptr<Light>, Spectrum>> Lights;
    BxDFs.push_back(make_pair(bxdf, spectrum));
    return make_shared<Surface>(BxDFs, Lights);
}

pair<int, int> Surface::surface_info() const {
    return make_pair(BxDFs.size(), Lights.size());
}

void Surface::evaluate_VtS(const Vec3 &inter, const Vec3 &N, const Vec3 &V, Spectrum &spectrum) {
    Mat3 T, T_I;
    Vec3 V_;
    rotateAxis(N, V, T, T_I);
    V_ = T * V;
    spectrum = Spectrum();
    for (auto Func: Lights) {
        double weight;
        get<0>(Func)->evaluate(V_, weight);
        spectrum = spectrum + get<1>(Func) * Spectrum(weight);
    }
}

void Surface::evaluate_StV(const Vec3 &inter, const Vec3 &N, const Vec3 &V, Spectrum &spectrum) {
    Mat3 T, T_I;
    Vec3 V_;
    rotateAxis(N, V, T, T_I);
    V_ = T * V;
    spectrum = Spectrum();
    for (auto Func: Lights) {
        double weight;
        get<0>(Func)->evaluate(V_, weight);
        spectrum = spectrum + get<1>(Func) * Spectrum(weight);
    }
    assert(abs(V_.length() - 1) < EPS);
    spectrum = spectrum * abs(V_.z());
}

void Surface::evaluate_VtL(const Vec3 &inter, const Vec3 &N, const Vec3 &V, const Vec3 &L, Spectrum &spectrum) {
    Mat3 T, T_I;
    Vec3 V_, L_;
    rotateAxis(N, V, T, T_I);
    V_ = T * V;
    L_ = T * L;

//    cerr << "evaluate VtL" << " " << V.length() << " " << L.length() << " " << V_.length() << " " << L_.length() << endl;
//    cerr << "evaluate VtL" << " " << V_ << " " << L_ << endl;
    spectrum = Spectrum();
    for (auto Func: BxDFs) {
        double weight;
        get<0>(Func)->evaluate(V_, L_, weight);
        spectrum = spectrum + get<1>(Func) * Spectrum(weight);
    }
    assert(abs(L_.length() - 1) < EPS);
    spectrum = spectrum * abs(L_.z());
}

void Surface::evaluate_LtV(const Vec3 &inter, const Vec3 &N, const Vec3 &L, const Vec3 &V, Spectrum &spectrum) {
    Mat3 T, T_I;
    Vec3 V_, L_;
    rotateAxis(N, V, T, T_I);
    V_ = T * V;
    L_ = T * L;
    spectrum = Spectrum();
    for (auto Func: BxDFs) {
        double weight;
        get<0>(Func)->evaluate(V_, L_, weight);
        spectrum = spectrum + get<1>(Func) * Spectrum(weight);
    }
    assert(abs(L_.length() - 1) < EPS);
    spectrum = spectrum * abs(V_.z());
}

void Surface::sample_VtL(const Vec3 &inter, const Vec3 &N, const Vec3 &V, Vec3 &L, double &pdf) {
    Mat3 T, T_I;
    Vec3 V_, L_;
    rotateAxis(N, V, T, T_I);
    V_ = T * V;
    if (BxDFs.size() == 0) L_ = RD.sphere(), pdf = 1 / (4 * pi);
    else get<0>(BxDFs[0])->sample_VtL(V_, L_, pdf);
    L = T_I * L_;
}

void Surface::sample_LtV(const Vec3 &inter, const Vec3 &N, const Vec3 &L, Vec3 &V, double &pdf) {
    Mat3 T, T_I;
    Vec3 V_, L_;
    rotateAxis(N, L, T, T_I);
    L_ = T * L;
    if (BxDFs.size() == 0) V_ = RD.sphere(), pdf = 1 / (4 * pi);
    else get<0>(BxDFs[0])->sample_LtV(L_, V_, pdf);
    V = T_I * V_;
}

void Surface::sample_StV(const Vec3 &inter, const Vec3 &N, Vec3 &V, double &pdf) {
    Mat3 T, T_I;
    Vec3 V_;
    rotateAxis(N, N, T, T_I);
    if (Lights.size() == 0) V_ = RD.sphere(), pdf = 1 / (4 * pi);
    else get<0>(Lights[0])->sample(V_, pdf);
    V = T_I * V_;
}
#endif
#endif /* surface_hpp */
