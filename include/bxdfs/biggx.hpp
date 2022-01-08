#ifndef bxdfs_biggx_hpp
#define bxdfs_biggx_hpp

#include "../bxdf.hpp"

class BiGGX : public BxDF {
public:
    BiGGX(double ior, double roughness);

private:
    Spectrum evaluate(const Vec2 &texPos, const Vec3 &i, const Vec3 &o) const override;
    Spectrum sample(const Vec2 &texPos, const Vec3 &i, Vec3 &o, double &pdf, Sampler &RNG) const override;
    double evaluateImportance(const Vec2 &texPos, const Vec3 &i, const Vec3 &o) const override;

    Spectrum F(const Vec3 &i, const Vec3 &o, const Vec3 &n) const;
    double D(const Vec3 &n) const;
    double G(const Vec3 &i, const Vec3 &o, const Vec3 &n) const;
    void sampleN(Vec3 &n, double &pdf, Sampler &RNG) const;

    double ior;
    double rough;
};

#ifdef ARC_IMPLEMENTATION

BiGGX::BiGGX(double ior, double roughness) : ior(ior), rough(roughness) {}

Spectrum BiGGX::F(const Vec3 &i, const Vec3 &o, const Vec3 &n) const {
    Spectrum F0(pow((1 - ior) / (1 + ior), 2));
    if (o.z() > 0)
        return F0 + (Spectrum(1) - F0) * pow(1 - abs(o * n), 5);
    else {
        return F0 + (Spectrum(1) - F0) * pow(1 - sqrt(max(0., (pow(o * n, 2) - 1) * pow(ior, 2) + 1)), 5);
    }
}

double BiGGX::D(const Vec3 &n) const {
    double alpha = rough * rough;
    double alpha2 = alpha * alpha;
    return alpha2 / (pi * pow(n.z() * n.z() * (alpha2 - 1) + 1, 2));
}

double BiGGX::G(const Vec3 &i, const Vec3 &o, const Vec3 &n) const {
    double alpha = rough * rough;
    double alpha2 = alpha * alpha;
    double tan_v = (i.z() > 0 ? 1 : -1) * sqrt(1 - (i.z()) * (i.z())) / (i.z());
    double tan_l = (o.z() > 0 ? 1 : -1) * sqrt(1 - (o.z()) * (o.z())) / (o.z());
    double G_v = 2 / (1 + sqrt(1 + alpha2 * tan_v * tan_v));
    double G_l = 2 / (1 + sqrt(1 + alpha2 * tan_l * tan_l));
    return max(G_v, 0.) * max(G_l, 0.);
}

void BiGGX::sampleN(Vec3 &n, double &pdf, Sampler &RNG) const {
    double alpha = rough * rough;
    double alpha2 = alpha * alpha;
    double p = RNG.rand(), q = RNG.rand(-pi, pi);
    double cos_n = sqrt((1 - p) / (p * (alpha2 - 1) + 1));
    double sin_n = sqrt(1 - cos_n * cos_n);
    n = Vec3(cos(q) * sin_n, sin(q) * sin_n, cos_n);
    pdf = alpha2 * cos_n / (pi * pow((alpha2 - 1) * cos_n * cos_n + 1, 2));
}

Spectrum BiGGX::evaluate(const Vec2 &texPos, const Vec3 &i, const Vec3 &o) const {
    if (i.z() * o.z() > 0) {
        Vec3 n = (i + o).norm();

//        cerr << "Reflection" << endl;
//        cerr << F(vLocal, lLocal, n) << endl;
//        cerr << D(n) << " " << G(vLocal, lLocal, n) << endl;
//        cerr << 1. / (4 * vLocal.z() * lLocal.z()) << endl;

        return F(i, o, n) * D(n) * G(i, o, n) / (4 * i.z() * o.z());
    } else {
        Vec3 n = (o + i * (o.z() > 0 ? ior : 1 / ior)).norm();

//        cerr << "Transmission" << endl;
//        cerr << (Spectrum(1) - F(vLocal, lLocal, n)) << endl;
//        cerr << D(n) << " " << G(vLocal, lLocal, n) << endl;
//        cerr << (abs(vLocal * n) * abs(lLocal * n)) / abs(vLocal.z() * lLocal.z())
//            / (lLocal + vLocal * (lLocal.z() > 0 ? ior : 1 / ior)).squaredLength() << endl;

        return (Spectrum(1) - F(i, o, n)) * D(n) * G(i, o, n)
            * (abs(i * n) * abs(o * n)) / abs(i.z() * o.z())
            / (o + i * (o.z() > 0 ? ior : 1 / ior)).squaredLength();
    }
}

Spectrum BiGGX::sample(const Vec2 &texPos, const Vec3 &i, Vec3 &o, double &pdf, Sampler &RNG) const {
    Vec3 n;
    sampleN(n, pdf, RNG);
    o = -i + n * (i * n) * 2;
    double prob = (rough + F(i, o, n).norm() / sqrt(3)) / (1 + 2 * rough);
    if (RNG.rand() < prob) {
        o = -i + n * (i * n) * 2;
        pdf = pdf / abs(4 * i * n) * prob;
        if (o.z() * i.z() > 0) return evaluate(texPos, i, o);
        else return Spectrum(0);
    } else {
        double cosV = abs(i * n), sinV = sqrt(1 - cosV * cosV);
        double sinL = sinV * (i.z() < 0 ? ior : 1 / ior);
        if (sinL > 1) {
            pdf = 1.;
            return Spectrum(0);
        }
        if (n * i < 0) n = -n;
        o = -(n + i.vert(n).norm() * tan(asin(sinL))).norm();

//        Vec3 nx = (lLocal + vLocal * (lLocal.z() > 0 ? ior : 1 / ior)).norm();
//        if ((nx - n).length() > 1e-5) cerr << "?L" << nx << " " << n << endl;
//        if ((n * lLocal) * (n * vLocal) > 0) cerr << "? ? ? ? ? ? ? ? ?  ? ? ? ? ? ? ? ? ? ? ?? ? ? ? ? ? " << endl;

        pdf = pdf * abs(o * n) / (o + i * (i.z() < 0 ? ior : 1 / ior)).squaredLength() * (1 - prob);
        if (o.z() * i.z() > 0) return Spectrum(0);
        else return evaluate(texPos, i, o);
    }
}

double BiGGX::evaluateImportance(const Vec2 &texPos, const Vec3 &i, const Vec3 &o) const {
    if (i.z() * o.z() > 0) {
        Vec3 n = (i + o).norm();
        double prob = (rough + F(i, o, n).norm() / sqrt(3)) / (1 + 2 * rough);
        return D(n) * prob;
    } else {
        Vec3 n = (o + i * (o.z() > 0 ? ior : 1 / ior)).norm();
        double prob = (rough + F(i, o, n).norm() / sqrt(3)) / (1 + 2 * rough);
        return D(n) * (1 - prob);
    }
}


#endif
#endif /* bxdfs_biggx_hpp */
