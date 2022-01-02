#ifndef bxdfs_biggx_hpp
#define bxdfs_biggx_hpp

#include "../bxdf.hpp"

class BiGGX : public BxDF {
public:
    BiGGX(double ior, double rough);

private:
    Spectrum evaluate(const Vec3 &vLocal, const Vec3 &lLocal) const override;
    Spectrum sample(const Vec3 &vLocal, Vec3 &lLocal, double &pdf, Sampler &RNG) const override;
    double evaluateImportance(const Vec3 &vLocal, const Vec3 &lLocal) const override;

    Spectrum F(const Vec3 &V, const Vec3 &L, const Vec3 &N) const;
    double D(const Vec3 &N) const;
    double G(const Vec3 &V, const Vec3 &L, const Vec3 &N) const;
    void sampleN(Vec3 &N, double &pdf, Sampler &RNG) const;

    double ior;
    double rough;
};

#ifdef ARC_IMPLEMENTATION

BiGGX::BiGGX(double ior, double rough) : ior(ior), rough(rough) {}

Spectrum BiGGX::F(const Vec3 &V, const Vec3 &L, const Vec3 &N) const {
    Spectrum F0(pow((1 - ior) / (1 + ior), 2));
    if (L.z() > 0)
        return F0 + (Spectrum(1) - F0) * pow(1 - abs(L * N), 5);
    else {
        return F0 + (Spectrum(1) - F0) * pow(1 - sqrt(max(0., (pow(L * N, 2) - 1) * pow(ior, 2) + 1)), 5);
    }
}

double BiGGX::D(const Vec3 &N) const {
    double alpha = rough * rough;
    double alpha2 = alpha * alpha;
    return alpha2 / (pi * pow(N.z() * N.z() * (alpha2 - 1) + 1, 2));
}

double BiGGX::G(const Vec3 &V, const Vec3 &L, const Vec3 &N) const {
    double alpha = rough * rough;
    double alpha2 = alpha * alpha;
    double tan_v = (V.z() > 0 ? 1 : -1) * sqrt(1 - (V.z()) * (V.z())) / (V.z());
    double tan_l = (L.z() > 0 ? 1 : -1) * sqrt(1 - (L.z()) * (L.z())) / (L.z());
    double G_v = 2 / (1 + sqrt(1 + alpha2 * tan_v * tan_v));
    double G_l = 2 / (1 + sqrt(1 + alpha2 * tan_l * tan_l));
    return max(G_v, 0.) * max(G_l, 0.);
}

void BiGGX::sampleN(Vec3 &N, double &pdf, Sampler &RNG) const {
    double alpha = rough * rough;
    double alpha2 = alpha * alpha;
    double p = RNG.rand(), q = RNG.rand(-pi, pi);
    double cos_n = sqrt((1 - p) / (p * (alpha2 - 1) + 1));
    double sin_n = sqrt(1 - cos_n * cos_n);
    N = Vec3(cos(q) * sin_n, sin(q) * sin_n, cos_n);
    pdf = alpha2 * cos_n / (pi * pow((alpha2 - 1) * cos_n * cos_n + 1, 2));
}

Spectrum BiGGX::evaluate(const Vec3 &vLocal, const Vec3 &lLocal) const {
    if (vLocal.z() * lLocal.z() > 0) {
        Vec3 n = (vLocal + lLocal).norm();

//        cerr << "Reflection" << endl;
//        cerr << F(vLocal, lLocal, n) << endl;
//        cerr << D(n) << " " << G(vLocal, lLocal, n) << endl;
//        cerr << 1. / (4 * vLocal.z() * lLocal.z()) << endl;

        return F(vLocal, lLocal, n) * D(n) * G(vLocal, lLocal, n) / (4 * vLocal.z() * lLocal.z());
    } else {
        Vec3 n = (lLocal + vLocal * (lLocal.z() > 0 ? ior : 1 / ior)).norm();

//        cerr << "Transmission" << endl;
//        cerr << (Spectrum(1) - F(vLocal, lLocal, n)) << endl;
//        cerr << D(n) << " " << G(vLocal, lLocal, n) << endl;
//        cerr << (abs(vLocal * n) * abs(lLocal * n)) / abs(vLocal.z() * lLocal.z())
//            / (lLocal + vLocal * (lLocal.z() > 0 ? ior : 1 / ior)).squaredLength() << endl;

        return (Spectrum(1) - F(vLocal, lLocal, n)) * D(n) * G(vLocal, lLocal, n)
            * (abs(vLocal * n) * abs(lLocal * n)) / abs(vLocal.z() * lLocal.z())
            / (lLocal + vLocal * (lLocal.z() > 0 ? ior : 1 / ior)).squaredLength();
    }
}

Spectrum BiGGX::sample(const Vec3 &vLocal, Vec3 &lLocal, double &pdf, Sampler &RNG) const {
    Vec3 n;
    sampleN(n, pdf, RNG);
    lLocal = -vLocal + n * (vLocal * n) * 2;
    double prob = (rough + F(vLocal, lLocal, n).norm() / sqrt(3)) / (1 + 2 * rough);
    if (RNG.rand() < prob) {
        lLocal = -vLocal + n * (vLocal * n) * 2;
        pdf = pdf / abs(4 * vLocal * n) * prob;
        if (lLocal.z() * vLocal.z() > 0) return evaluate(vLocal, lLocal);
        else return Spectrum(0);
    } else {
        double cosV = abs(vLocal * n), sinV = sqrt(1 - cosV * cosV);
        double sinL = sinV * (vLocal.z() < 0 ? ior : 1 / ior);
        if (sinL > 1) {
            pdf = 1.;
            return Spectrum(0);
        }
        if (n * vLocal < 0) n = -n;
        lLocal = -(n + vLocal.vert(n).norm() * tan(asin(sinL))).norm();

//        Vec3 nx = (lLocal + vLocal * (lLocal.z() > 0 ? ior : 1 / ior)).norm();
//        if ((nx - n).length() > 1e-5) cerr << "?L" << nx << " " << n << endl;
//        if ((n * lLocal) * (n * vLocal) > 0) cerr << "? ? ? ? ? ? ? ? ?  ? ? ? ? ? ? ? ? ? ? ?? ? ? ? ? ? " << endl;

        pdf = pdf * abs(lLocal * n) / (lLocal + vLocal * (vLocal.z() < 0 ? ior : 1 / ior)).squaredLength() * (1 - prob);
        if (lLocal.z() * vLocal.z() > 0) return Spectrum(0);
        else return evaluate(vLocal, lLocal);
    }
}

double BiGGX::evaluateImportance(const Vec3 &vLocal, const Vec3 &lLocal) const {
    if (vLocal.z() * lLocal.z() > 0) {
        Vec3 n = (vLocal + lLocal).norm();
        double prob = (rough + F(vLocal, lLocal, n).norm() / sqrt(3)) / (1 + 2 * rough);
        return D(n) * prob;
    } else {
        Vec3 n = (lLocal + vLocal * (lLocal.z() > 0 ? ior : 1 / ior)).norm();
        double prob = (rough + F(vLocal, lLocal, n).norm() / sqrt(3)) / (1 + 2 * rough);
        return D(n) * (1 - prob);
    }
}


#endif
#endif /* bxdfs_biggx_hpp */
