#ifndef bxdfs_mtl_hpp
#define bxdfs_mtl_hpp

#include "../bxdf.hpp"

class MtlGGX : public BxDF {
public:
    MtlGGX(Spectrum diffuse, Spectrum specular, double rough, double ior, double dissolve);

private:
    Spectrum evaluate(const Vec3 &vLocal, const Vec3 &lLocal) const override;
    Spectrum sample(const Vec3 &vLocal, Vec3 &lLocal, double &pdf, Sampler &RNG) const override;
    double evaluateImportance(const Vec3 &vLocal, const Vec3 &lLocal) const override;

    Spectrum F(const Vec3 &V, const Vec3 &L, const Vec3 &N) const;
    double D(const Vec3 &N) const;
    double G(const Vec3 &V, const Vec3 &L, const Vec3 &N) const;
    void sampleN(Vec3 &N, double &pdf, Sampler &RNG) const;

    Spectrum diffuse, specular;
    double rough, ior, dissolve;
};

#ifdef ARC_IMPLEMENTATION

MtlGGX::MtlGGX(Spectrum diffuse, Spectrum specular, double rough, double ior, double dissolve)
    : diffuse(diffuse), specular(specular), rough(rough), ior(ior), dissolve(dissolve) {}

Spectrum MtlGGX::F(const Vec3 &V, const Vec3 &L, const Vec3 &N) const {
    if (L.z() > 0)
        return specular + (Spectrum(1) - specular) * pow(1 - abs(L * N), 5);
    else {
        return specular + (Spectrum(1) - specular) * pow(1 - sqrt(max(0., (pow(L * N, 2) - 1) * pow(ior, 2) + 1)), 5);
    }
}

double MtlGGX::D(const Vec3 &N) const {
    double alpha = rough * rough;
    double alpha2 = alpha * alpha;
    return alpha2 / (pi * pow(N.z() * N.z() * (alpha2 - 1) + 1, 2));
}

double MtlGGX::G(const Vec3 &V, const Vec3 &L, const Vec3 &N) const {
    double alpha = rough * rough;
    double alpha2 = alpha * alpha;
    double tan_v = (V.z() > 0 ? 1 : -1) * sqrt(1 - (V.z()) * (V.z())) / (V.z());
    double tan_l = (L.z() > 0 ? 1 : -1) * sqrt(1 - (L.z()) * (L.z())) / (L.z());
    double G_v = 2 / (1 + sqrt(1 + alpha2 * tan_v * tan_v));
    double G_l = 2 / (1 + sqrt(1 + alpha2 * tan_l * tan_l));
    return max(G_v, 0.) * max(G_l, 0.);
}

void MtlGGX::sampleN(Vec3 &N, double &pdf, Sampler &RNG) const {
    double alpha = rough * rough;
    double alpha2 = alpha * alpha;
    double p = RNG.rand(), q = RNG.rand(-pi, pi);
    double cos_n = sqrt((1 - p) / (p * (alpha2 - 1) + 1));
    double sin_n = sqrt(1 - cos_n * cos_n);
    N = Vec3(cos(q) * sin_n, sin(q) * sin_n, cos_n);
    pdf = alpha2 * cos_n / (pi * pow((alpha2 - 1) * cos_n * cos_n + 1, 2));
}

Spectrum MtlGGX::evaluate(const Vec3 &vLocal, const Vec3 &lLocal) const {
    if (vLocal.z() * lLocal.z() > 0) {
        Vec3 n = (vLocal + lLocal).norm();

        return F(vLocal, lLocal, n) * D(n) * G(vLocal, lLocal, n) / (4 * vLocal.z() * lLocal.z())
            + diffuse / pi * dissolve;
    } else {
        Vec3 n = (lLocal + vLocal * (lLocal.z() > 0 ? ior : 1 / ior)).norm();

        return (Spectrum(1) - F(vLocal, lLocal, n)) * D(n) * G(vLocal, lLocal, n)
            * (abs(vLocal * n) * abs(lLocal * n)) / abs(vLocal.z() * lLocal.z())
            / (lLocal + vLocal * (lLocal.z() > 0 ? ior : 1 / ior)).squaredLength() * (1 - dissolve);
    }
}

Spectrum MtlGGX::sample(const Vec3 &vLocal, Vec3 &lLocal, double &pdf, Sampler &RNG) const {
    Vec3 n;
    sampleN(n, pdf, RNG);
    lLocal = -vLocal + n * (vLocal * n) * 2;
    double prob = (rough + F(vLocal, lLocal, n).norm() / sqrt(3)) / (1 + 2 * rough);
    if (RNG.rand() < prob) {
        lLocal = -vLocal + n * (vLocal * n) * 2;
        pdf = pdf / abs(4 * vLocal * n) * prob;
        if (lLocal.z() * vLocal.z() > 0)
            return F(vLocal, lLocal, n) * D(n) * G(vLocal, lLocal, n) / (4 * vLocal.z() * lLocal.z());
        else return Spectrum(0);
    } else if (RNG.rand() < dissolve) {
        lLocal = RNG.hemisphere();
        pdf = 1 / (2 * pi) * (1 - prob) * dissolve;
        return diffuse / pi;
    } else {
        double cosV = abs(vLocal * n), sinV = sqrt(1 - cosV * cosV);
        double sinL = sinV * (vLocal.z() < 0 ? ior : 1 / ior);
        if (sinL > 1) {
            pdf = 1.;
            return Spectrum(0);
        }
        if (n * vLocal < 0) n = -n;
        lLocal = -(n + vLocal.vert(n).norm() * tan(asin(sinL))).norm();

        pdf = pdf * abs(lLocal * n) / (lLocal + vLocal * (vLocal.z() < 0 ? ior : 1 / ior)).squaredLength()
            * (1 - prob) * (1 - dissolve);
        if (lLocal.z() * vLocal.z() > 0) return Spectrum(0);
        else return (Spectrum(1) - F(vLocal, lLocal, n)) * D(n) * G(vLocal, lLocal, n)
                * (abs(vLocal * n) * abs(lLocal * n)) / abs(vLocal.z() * lLocal.z())
                / (lLocal + vLocal * (lLocal.z() > 0 ? ior : 1 / ior)).squaredLength();
    }
}

double MtlGGX::evaluateImportance(const Vec3 &vLocal, const Vec3 &lLocal) const {
    if (vLocal.z() * lLocal.z() > 0) {
        Vec3 n = (vLocal + lLocal).norm();
        double prob = (rough + F(vLocal, lLocal, n).norm() / sqrt(3)) / (1 + 2 * rough);
        return D(n) * prob + 1 / (2 * pi) * (1 - prob) * dissolve;
    } else {
        Vec3 n = (lLocal + vLocal * (lLocal.z() > 0 ? ior : 1 / ior)).norm();
        double prob = (rough + F(vLocal, lLocal, n).norm() / sqrt(3)) / (1 + 2 * rough);
        return D(n) * (1 - prob) * (1 - dissolve);
    }
}


#endif
#endif /* bxdfs_mtl_hpp */
