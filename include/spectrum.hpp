#ifndef spectrum_hpp
#define spectrum_hpp

#include "utils.hpp"
#include "mapping.hpp"

struct Spectrum
{
    double r, g, b;

    Spectrum (double l = 0);
    Spectrum (double _r, double _g, double _b);
    Spectrum operator + (const Spectrum &v) const;
    Spectrum operator - (const Spectrum &v) const;
    Spectrum operator * (const Spectrum &v) const;
    Spectrum operator / (double v) const;
    Spectrum operator ^ (double v) const;
    int rgb888(double white, double gamma = 2.2) const;
    double norm() const;

    friend ostream& operator << (ostream &s, Spectrum t);
};

Spectrum rgb888(double l = 0, double gamma = 2.2);
Spectrum rgb888(double r, double g, double b, double gamma = 2.2);
Spectrum rgb(double r, double g, double b, double gamma = 2.2);
Spectrum rgb(const Pixel &t, double gamma = 2.2);

#ifdef ARC_IMPLEMENTATION


Spectrum::Spectrum (double l)
{
    r = g = b = l;
}
Spectrum::Spectrum (double _r, double _g, double _b)
{
    r = _r; g = _g; b = _b;
}

Spectrum Spectrum::operator + (const Spectrum &v) const
{
    return Spectrum(r + v.r, g + v.g, b + v.b);
}
Spectrum Spectrum::operator - (const Spectrum &v) const
{
    return Spectrum(r - v.r, g - v.g, b - v.b);
}
Spectrum Spectrum::operator * (const Spectrum &v) const
{
    return Spectrum(r * v.r, g * v.g, b * v.b);
}
Spectrum Spectrum::operator / (double v) const
{
    if (v == 0) return Spectrum(0);
    return Spectrum(r / v, g / v, b / v);
}
Spectrum Spectrum::operator ^ (double v) const
{
    return Spectrum(pow(r, v), pow(g, v), pow(b, v));
}

int Spectrum::rgb888(double white, double gamma) const
{
    int _r = min(pow(r, 1 / gamma) / white, (double)1.) * 255,
        _g = min(pow(g, 1 / gamma) / white, (double)1.) * 255,
        _b = min(pow(b, 1 / gamma) / white, (double)1.) * 255;
    return (_r * 256 + _g) * 256 + _b;
}

double Spectrum::norm() const
{
    return sqrt(r * r + g * g + b * b);
}

ostream& operator << (ostream &s, Spectrum t)
{
    s << "(" << t.r << "," << t.g << "," << t.b << ")";
    return s;
}

Spectrum rgb888(double l, double gamma)
{
    return Spectrum(pow(l / 255, gamma));
}
Spectrum rgb888(double r, double g, double b, double gamma)
{
    return Spectrum(pow(r / 255, gamma), pow(g / 255, gamma), pow(b / 255, gamma));
}
Spectrum rgb(double r, double g, double b, double gamma)
{
    return Spectrum(pow(r, gamma), pow(g, gamma), pow(b, gamma));
}
Spectrum rgb(const Pixel &t, double gamma)
{
    return Spectrum(pow(t.r, gamma), pow(t.g, gamma), pow(t.b, gamma));
}

#endif
#endif /* spectrum_hpp */
