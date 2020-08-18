#ifndef sampler_hpp
#define sampler_hpp

#include "utils.hpp"
#include "geometry.hpp"

template<int base>
class HaltonSequence
{
    long long key;
    double now;

public:
    HaltonSequence()
    {
        key = 0;
        now = 0;
    }
    double operator ()()
    {
        double q = 1. / base;
        key += 1;
        for (long long i = key; i % base == 0; now -= (base - 1) * q, i /= base, q /= base);
        now += q;
        return now;
    }
};

class Sampler
{
    mt19937 R;

public:
    double sample();
    double rand(double l = 0, double r = 1);

    Vec2 pixel(Vec2 t, double width, double height);
    Vec3 semisphere();
    Vec3 sphere();
};

static Sampler RD;

#ifdef ARC_IMPLEMENTATION

double Sampler::sample()
{
    double result = 1. * R() / (1ll << 32);
    return result;
}
double Sampler::rand(double l, double r)
{
    return sample() * (r - l) + l;
}

Vec2 Sampler::pixel(Vec2 t, double width, double height)
{
    double _x = sample(), _y = sample();
    double dx = (t.d[0] + _x) / width - 0.5, dy = (t.d[1] + _y) / height - 0.5;
    return Vec2(dx, dy);
}
Vec3 Sampler::semisphere()
{
    auto v = make_pair(sample(), sample());
    double z = v.first, phi = v.second * (2 * pi) - pi;
    return Vec3(sin(phi) * sqrt(1 - z * z), cos(phi) * sqrt(1 - z * z), z);
}

Vec3 Sampler::sphere()
{
    auto v = make_pair(sample(), sample());
    double z = v.first * 2 - 1, phi = v.second * (2 * pi) - pi;
    return Vec3(sin(phi) * sqrt(1 - z * z), cos(phi) * sqrt(1 - z * z), z);
}

#endif
#endif /* sampler_hpp */
