#ifndef SAMPLER
#define SAMPLER
#include <random>
#include <cmath>
#include "geo.hpp"
using namespace std;

template<int base>
class HaltonSequence
{
    long long key; double now;

public:
    HaltonSequence ()
    {
        key = 0;
        now = 0;
    }
    double operator () ()
    {
        double q = 1. / base;
        key += 1;
        for (int i = key; i % base == 0; now -= (base - 1) * q, i /= base, q /= base);
        now += q;
        return now;
    }
};

class Sampler
{
    mt19937 R;
    HaltonSequence<7> X;
    HaltonSequence<11> Y;

public:
    Sampler () {}

    double sample() { return 1. * R() / (1ll << 32); }
    double rand(double l = 0, double r = 1)
    {
        return sample() * (r - l) + l;
    }

    pair<double,double> pixel(double x, double y, double width, double height)
    {
        pair<double,double> v = make_pair(X(), Y());
        double dx = (x + v.first) / width - 0.5, dy = (y + v.second) / height - 0.5;
        return make_pair(dx, dy);
    }
    Vec3 semisphere()
    {
        pair<double,double> v = make_pair(rand(), rand());
        double z = v.first, phi = v.second * (2 * pi) - pi;
        return Vec3(sin(phi) * sqrt(1 - z * z), cos(phi) * sqrt(1 - z * z), z);
    }
} RD;

#endif
