#ifndef bxdfs_mtlExt_hpp
#define bxdfs_mtlExt_hpp

#include <utility>

#include "../bxdf.hpp"

class MtlExtGGX : public BxDF {
public:
    MtlExtGGX(TextureMap baseColor, TextureMap metallic, TextureMap roughness, double ior, double dissolve,
        TextureMap normalMap = TextureMap(Spectrum(0, 0, 1)));

private:
    Spectrum evaluate(const Vec2 &texPos, const Vec3 &i, const Vec3 &o) const override;
    Spectrum sample(const Vec2 &texPos, const Vec3 &i, Vec3 &o, double &pdf, Sampler &RNG) const override;
    double evaluateImportance(const Vec2 &texPos, const Vec3 &i, const Vec3 &o) const override;

    Spectrum F(const Vec2 &texPos, const Vec3 &i, const Vec3 &o, const Vec3 &n) const;
    double D(const Vec2 &texPos, const Vec3 &n) const;
    double G(const Vec2 &texPos, const Vec3 &i, const Vec3 &o, const Vec3 &n) const;
    void sampleN(const Vec2 &texPos, Vec3 &n, double &pdf, Sampler &RNG) const;

    TextureMap baseColor, metallic, roughness, normalMap;
    double ior, dissolve;
};

#ifdef ARC_IMPLEMENTATION

MtlExtGGX::MtlExtGGX(TextureMap baseColor, TextureMap metallic, TextureMap roughness, double ior, double dissolve, TextureMap normalMap)
    : baseColor(baseColor), metallic(metallic), roughness(roughness), ior(ior), dissolve(dissolve), normalMap(normalMap) {}

Spectrum MtlExtGGX::F(const Vec2 &texPos, const Vec3 &i, const Vec3 &o, const Vec3 &n) const {
    if (o.z() > 0)
        return baseColor[texPos] + (Spectrum(1) - baseColor[texPos]) * pow(1 - abs(o * n), 5);
    else {
        return baseColor[texPos] + (Spectrum(1) - baseColor[texPos]) * pow(1 - sqrt(max(0., (pow(o * n, 2) - 1) * pow(ior, 2) + 1)), 5);
    }
}

double MtlExtGGX::D(const Vec2 &texPos, const Vec3 &n) const {
    Vec3 m = Vec3(normalMap[texPos].r, normalMap[texPos].g, normalMap[texPos].b).norm();

    double alpha = pow(roughness[texPos].norm(), 2);
    double alpha2 = alpha * alpha;
    return alpha2 / (pi * pow(pow(n * m, 2) * (alpha2 - 1) + 1, 2));
}

double MtlExtGGX::G(const Vec2 &texPos, const Vec3 &i, const Vec3 &o, const Vec3 &n) const {
    double alpha = pow(roughness[texPos].norm(), 2);
    double alpha2 = alpha * alpha;
    double tan_v = (i.z() > 0 ? 1 : -1) * sqrt(1 - (i.z()) * (i.z())) / (i.z());
    double tan_l = (o.z() > 0 ? 1 : -1) * sqrt(1 - (o.z()) * (o.z())) / (o.z());
    double G_v = 2 / (1 + sqrt(1 + alpha2 * tan_v * tan_v));
    double G_l = 2 / (1 + sqrt(1 + alpha2 * tan_l * tan_l));
    return max(G_v, 0.) * max(G_l, 0.);
}

void MtlExtGGX::sampleN(const Vec2 &texPos, Vec3 &n, double &pdf, Sampler &RNG) const {
    double alpha = pow(roughness[texPos].norm(), 2);
    double alpha2 = alpha * alpha;
    double p = RNG.rand(), q = RNG.rand(-pi, pi);
    double cos_n = sqrt((1 - p) / (p * (alpha2 - 1) + 1));
    double sin_n = sqrt(1 - cos_n * cos_n);
    Vec3 lN = Vec3(cos(q) * sin_n, sin(q) * sin_n, cos_n);
    pdf = alpha2 * cos_n / (pi * pow((alpha2 - 1) * cos_n * cos_n + 1, 2));

    Vec3 m = Vec3(normalMap[texPos].r, normalMap[texPos].g, normalMap[texPos].b).norm();
    Mat3 T, TInv;
    rotateAxis(m, m, T, TInv);
    n = TInv * lN;
//#pragma omp critical
//    cerr << lN.z() << " " << (m * n) << endl;
}

Spectrum MtlExtGGX::evaluate(const Vec2 &texPos, const Vec3 &i, const Vec3 &o) const {
    if (i.z() * o.z() > 0) {
        Vec3 n = (i + o).norm();

        return F(texPos, i, o, n) * D(texPos, n) * G(texPos, i, o, n) / (4 * i.z() * o.z()) * metallic[texPos].norm()
            + baseColor[texPos] / pi * dissolve * (1 - metallic[texPos].norm());
    } else {
        Vec3 n = (o + i * (o.z() > 0 ? ior : 1 / ior)).norm();

        return (Spectrum(1) - F(texPos, i, o, n)) * D(texPos, n) * G(texPos, i, o, n)
            * (abs(i * n) * abs(o * n)) / abs(i.z() * o.z())
            / (o + i * (o.z() > 0 ? ior : 1 / ior)).squaredLength() * (1 - dissolve);
    }
}

Spectrum MtlExtGGX::sample(const Vec2 &texPos, const Vec3 &i, Vec3 &o, double &pdf, Sampler &RNG) const {
    Vec3 n;
    sampleN(texPos, n, pdf, RNG);
    o = -i + n * (i * n) * 2;
    double prob = (roughness[texPos].norm() + F(texPos, i, o, n).norm() * metallic[texPos].norm()) / (1 + 2 * roughness[texPos].norm());
    if (RNG.rand() < prob) {
        o = -i + n * (i * n) * 2;
        pdf = pdf / abs(4 * i * n) * prob;
        if (o.z() * i.z() > 0)
            return F(texPos, i, o, n) * D(texPos, n) * G(texPos, i, o, n) / (4 * i.z() * o.z()) * metallic[texPos].norm();
        else return Spectrum(0);
    } else if (RNG.rand() < dissolve) {
        o = RNG.hemisphere();
        if (i.z() * o.z() < 0) o[2] *= -1;
        pdf = 1 / (2 * pi) * (1 - prob) * dissolve;
        return baseColor[texPos] / pi * (1 - metallic[texPos].norm());
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
        else return (Spectrum(1) - F(texPos, i, o, n)) * D(texPos, n) * G(texPos, i, o, n)
                * (abs(i * n) * abs(o * n)) / abs(i.z() * o.z())
                / (o + i * (o.z() > 0 ? ior : 1 / ior)).squaredLength();
    }
}

double MtlExtGGX::evaluateImportance(const Vec2 &texPos, const Vec3 &i, const Vec3 &o) const {
    if (i.z() * o.z() > 0) {
        Vec3 n = (i + o).norm();
        double prob = (roughness[texPos].norm() + F(texPos, i, o, n).norm() * metallic[texPos].norm()) / (1 + 2 * roughness[texPos].norm());
        return D(texPos, n) * prob + 1 / (2 * pi) * (1 - prob) * dissolve;
    } else {
        Vec3 n = (o + i * (o.z() > 0 ? ior : 1 / ior)).norm();
        double prob = (roughness[texPos].norm() + F(texPos, i, o, n).norm() * metallic[texPos].norm()) / (1 + 2 * roughness[texPos].norm());
        return D(texPos, n) * (1 - prob) * (1 - dissolve);
    }
}


#endif
#endif /* bxdfs_mtlExt_hpp */
