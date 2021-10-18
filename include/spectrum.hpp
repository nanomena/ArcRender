#ifndef spectrum_hpp
#define spectrum_hpp

#include "utils.hpp"
#include "mapping.hpp"

struct Spect
{
    double r, g, b;

    explicit Spect(double l = 0);
    Spect(double _r, double _g, double _b);
    Spect operator +(const Spect &v) const;
    Spect operator -(const Spect &v) const;
    Spect operator *(double k) const;
    Spect operator *(const Spect &v) const;
    Spect operator /(double v) const;
    Spect operator ^(double v) const;
    int rgb888(double white, double gamma = 2.2) const;
    double norm() const;

    friend ostream &operator <<(ostream &s, Spect t);
};

Spect rgb888(double l = 0, double gamma = 2.2);
Spect rgb888(double r, double g, double b, double gamma = 2.2);
Spect rgb(double l, double gamma = 2.2);
Spect rgb(double r, double g, double b, double gamma = 2.2);
Spect rgb(const Pixel &t, double gamma = 2.2);

#ifdef ARC_IMPLEMENTATION

Spect::Spect(double l)
{
    r = g = b = l;
}
Spect::Spect(double _r, double _g, double _b)
{
    r = _r, g = _g, b = _b;
}

Spect Spect::operator +(const Spect &v) const
{
    return Spect(r + v.r, g + v.g, b + v.b);
}
Spect Spect::operator -(const Spect &v) const
{
    return Spect(r - v.r, g - v.g, b - v.b);
}
Spect Spect::operator *(const Spect &v) const
{
    return Spect(r * v.r, g * v.g, b * v.b);
}
Spect Spect::operator *(double k) const
{
    return Spect(r * k, g * k, b * k);
}
Spect Spect::operator /(double v) const
{
    if (v == 0) return Spect(0);
    return Spect(r / v, g / v, b / v);
}
Spect Spect::operator ^(double v) const
{
    return Spect(pow(r, v), pow(g, v), pow(b, v));
}

int Spect::rgb888(double white, double gamma) const
{
    int _r = static_cast<int>(min(pow(r, 1 / gamma) / white, 1.) * 255),
        _g = static_cast<int>(min(pow(g, 1 / gamma) / white, 1.) * 255),
        _b = static_cast<int>(min(pow(b, 1 / gamma) / white, 1.) * 255);
    return (_r * 256 + _g) * 256 + _b;
}

double Spect::norm() const
{
    return sqrt(r * r + g * g + b * b);
}

ostream &operator <<(ostream &s, Spect t)
{
    s << "(" << t.r << "," << t.g << "," << t.b << ")";
    return s;
}

Spect rgb888(double l, double gamma)
{
    return Spect(pow(l / 255, gamma));
}
Spect rgb888(double r, double g, double b, double gamma)
{
    return Spect(pow(r / 255, gamma), pow(g / 255, gamma), pow(b / 255, gamma));
}
Spect rgb(double l, double gamma)
{
    return Spect(pow(l, gamma));
}
Spect rgb(double r, double g, double b, double gamma)
{
    return Spect(pow(r, gamma), pow(g, gamma), pow(b, gamma));
}
Spect rgb(const Pixel &t, double gamma)
{
    return Spect(pow(t.r, gamma), pow(t.g, gamma), pow(t.b, gamma));
}

#endif
#endif /* spectrum_hpp */
