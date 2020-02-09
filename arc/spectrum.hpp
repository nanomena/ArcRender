#ifndef SPECTRUM
#define SPECTRUM
#include "arc.hpp"
#include "geometry.hpp"

struct Spectrum
{
    double r, g, b;
    Spectrum (double l = 0) { r = g = b = l; }
    Spectrum (double _r, double _g, double _b) { r = _r, g = _g, b = _b; }

    Spectrum operator + (const Spectrum &v) const { return Spectrum(r + v.r, g + v.g, b + v.b); }
    Spectrum operator - (const Spectrum &v) const { return Spectrum(r - v.r, g - v.g, b - v.b); }
    Spectrum operator * (const Spectrum &v) const { return Spectrum(r * v.r, g * v.g, b * v.b); }
    Spectrum operator / (double v) const
    {
        if (v == 0) return Spectrum(0);
        return Spectrum(r / v, g / v, b / v);
    }
    Spectrum operator ^ (double v) const { return Spectrum(pow(r, v), pow(g, v), pow(b, v)); }
    int rgb888(double white, double gamma = 2.2) const
    {
        int _r = min(pow(r, 1 / gamma) / white, (double)1.) * 255,
            _g = min(pow(g, 1 / gamma) / white, (double)1.) * 255,
            _b = min(pow(b, 1 / gamma) / white, (double)1.) * 255;
        return (_r * 256 + _g) * 256 + _b;
    }
    double norm() { return sqrt(r * r + g * g + b * b); }

    friend ostream& operator << (ostream &s, Spectrum t)
    {
        s << "(" << t.r << "," << t.g << "," << t.b << ")";
        return s;
    }
};

Spectrum rgb888(double l = 0, double gamma = 2.2)
{
    return Spectrum(pow(l / 255, gamma));
}
Spectrum rgb888(double r, double g, double b, double gamma = 2.2)
{
    return Spectrum(pow(r / 255, gamma), pow(g / 255, gamma), pow(b / 255, gamma));
}

#endif
