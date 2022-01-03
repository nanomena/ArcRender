#ifndef bxdfs_mtl_hpp
#define bxdfs_mtl_hpp

#include <utility>

#include "../bxdf.hpp"

class MtlGGX : public BxDF {
public:
    MtlGGX(TextureMap diffuse, TextureMap specular, double roughness, double ior, double dissolve);

private:
    Spectrum evaluate(const Vec3 &pos, const Vec3 &i, const Vec3 &o) const override;
    Spectrum sample(const Vec3 &pos, const Vec3 &i, Vec3 &o, double &pdf, Sampler &RNG) const override;
    double evaluateImportance(const Vec3 &pos, const Vec3 &i, const Vec3 &o) const override;

    Spectrum F(const Vec3 &pos, const Vec3 &i, const Vec3 &o, const Vec3 &n) const;
    double D(const Vec3 &n) const;
    double G(const Vec3 &i, const Vec3 &o, const Vec3 &n) const;
    void sampleN(Vec3 &n, double &pdf, Sampler &RNG) const;

    TextureMap diffuse, specular;
    double rough, ior, dissolve;
};

#ifdef ARC_IMPLEMENTATION

MtlGGX::MtlGGX(TextureMap diffuse, TextureMap specular, double roughness, double ior, double dissolve)
    : diffuse(std::move(diffuse)), specular(std::move(specular)), rough(roughness), ior(ior), dissolve(dissolve) {}

Spectrum MtlGGX::F(const Vec3 &pos, const Vec3 &i, const Vec3 &o, const Vec3 &n) const {
    if (o.z() > 0)
        return specular[pos] + (Spectrum(1) - specular[pos]) * pow(1 - abs(o * n), 5);
    else {
        return specular[pos] + (Spectrum(1) - specular[pos]) * pow(1 - sqrt(max(0., (pow(o * n, 2) - 1) * pow(ior, 2) + 1)), 5);
    }
}

double MtlGGX::D(const Vec3 &n) const {
    double alpha = rough * rough;
    double alpha2 = alpha * alpha;
    return alpha2 / (pi * pow(n.z() * n.z() * (alpha2 - 1) + 1, 2));
}

double MtlGGX::G(const Vec3 &i, const Vec3 &o, const Vec3 &n) const {
    double alpha = rough * rough;
    double alpha2 = alpha * alpha;
    double tan_v = (i.z() > 0 ? 1 : -1) * sqrt(1 - (i.z()) * (i.z())) / (i.z());
    double tan_l = (o.z() > 0 ? 1 : -1) * sqrt(1 - (o.z()) * (o.z())) / (o.z());
    double G_v = 2 / (1 + sqrt(1 + alpha2 * tan_v * tan_v));
    double G_l = 2 / (1 + sqrt(1 + alpha2 * tan_l * tan_l));
    return max(G_v, 0.) * max(G_l, 0.);
}

void MtlGGX::sampleN(Vec3 &n, double &pdf, Sampler &RNG) const {
    double alpha = rough * rough;
    double alpha2 = alpha * alpha;
    double p = RNG.rand(), q = RNG.rand(-pi, pi);
    double cos_n = sqrt((1 - p) / (p * (alpha2 - 1) + 1));
    double sin_n = sqrt(1 - cos_n * cos_n);
    n = Vec3(cos(q) * sin_n, sin(q) * sin_n, cos_n);
    pdf = alpha2 * cos_n / (pi * pow((alpha2 - 1) * cos_n * cos_n + 1, 2));
}

Spectrum MtlGGX::evaluate(const Vec3 &pos, const Vec3 &i, const Vec3 &o) const {
    if (i.z() * o.z() > 0) {
        Vec3 n = (i + o).norm();

        return F(pos, i, o, n) * D(n) * G(i, o, n) / (4 * i.z() * o.z())
            + diffuse[pos] / pi * dissolve;
    } else {
        Vec3 n = (o + i * (o.z() > 0 ? ior : 1 / ior)).norm();

        return (Spectrum(1) - F(pos, i, o, n)) * D(n) * G(i, o, n)
            * (abs(i * n) * abs(o * n)) / abs(i.z() * o.z())
            / (o + i * (o.z() > 0 ? ior : 1 / ior)).squaredLength() * (1 - dissolve);
    }
}

Spectrum MtlGGX::sample(const Vec3 &pos, const Vec3 &i, Vec3 &o, double &pdf, Sampler &RNG) const {
    Vec3 n;
    sampleN(n, pdf, RNG);
    o = -i + n * (i * n) * 2;
    double prob = (rough + F(pos, i, o, n).norm() / sqrt(3)) / (1 + 2 * rough);
    if (RNG.rand() < prob) {
        o = -i + n * (i * n) * 2;
        pdf = pdf / abs(4 * i * n) * prob;
        if (o.z() * i.z() > 0)
            return F(pos, i, o, n) * D(n) * G(i, o, n) / (4 * i.z() * o.z());
        else return Spectrum(0);
    } else if (RNG.rand() < dissolve) {
        o = RNG.hemisphere();
        pdf = 1 / (2 * pi) * (1 - prob) * dissolve;
        return diffuse[pos] / pi;
    } else {
        double cosV = abs(i * n), sinV = sqrt(1 - cosV * cosV);
        double sinL = sinV * (i.z() < 0 ? ior : 1 / ior);
        if (sinL > 1) {
            pdf = 1.;
            return Spectrum(0);
        }
        if (n * i < 0) n = -n;
        o = -(n + i.vert(n).norm() * tan(asin(sinL))).norm();

        pdf = pdf * abs(o * n) / (o + i * (i.z() < 0 ? ior : 1 / ior)).squaredLength()
            * (1 - prob) * (1 - dissolve);
        if (o.z() * i.z() > 0) return Spectrum(0);
        else return (Spectrum(1) - F(pos, i, o, n)) * D(n) * G(i, o, n)
                * (abs(i * n) * abs(o * n)) / abs(i.z() * o.z())
                / (o + i * (o.z() > 0 ? ior : 1 / ior)).squaredLength();
    }
}

double MtlGGX::evaluateImportance(const Vec3 &pos, const Vec3 &i, const Vec3 &o) const {
    if (i.z() * o.z() > 0) {
        Vec3 n = (i + o).norm();
        double prob = (rough + F(pos, i, o, n).norm() / sqrt(3)) / (1 + 2 * rough);
        return D(n) * prob + 1 / (2 * pi) * (1 - prob) * dissolve;
    } else {
        Vec3 n = (o + i * (o.z() > 0 ? ior : 1 / ior)).norm();
        double prob = (rough + F(pos, i, o, n).norm() / sqrt(3)) / (1 + 2 * rough);
        return D(n) * (1 - prob) * (1 - dissolve);
    }
}


#endif
#endif /* bxdfs_mtl_hpp */
