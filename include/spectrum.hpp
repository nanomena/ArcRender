#ifndef spectrum_hpp
#define spectrum_hpp

#include "utils.hpp"

struct Spectrum {
    double r, g, b;

    explicit Spectrum(double l = 0);
    Spectrum(double _r, double _g, double _b);
    Spectrum operator +(const Spectrum &v) const;
    Spectrum operator -(const Spectrum &v) const;
    Spectrum operator *(double k) const;
    Spectrum operator *(const Spectrum &v) const;
    Spectrum operator /(double v) const;
    Spectrum operator ^(double v) const;
    int rgb256(double white, double gamma = 2.2) const;
    double norm() const;

    Spectrum &operator +=(const Spectrum &v);
    Spectrum &operator -=(const Spectrum &v);
    Spectrum &operator *=(double f);
    Spectrum &operator /=(double f);

    friend ostream &operator <<(ostream &s, Spectrum t);
};

Spectrum rgb256(double l = 0, double gamma = 2.2);
Spectrum rgb256(double r, double g, double b, double gamma = 2.2);
Spectrum rgb(double l, double gamma = 2.2);
Spectrum rgb(double r, double g, double b, double gamma = 2.2);

#ifdef ARC_IMPLEMENTATION

Spectrum::Spectrum(double l) {
    r = g = b = l;
}
Spectrum::Spectrum(double _r, double _g, double _b) {
    r = _r, g = _g, b = _b;
}

Spectrum Spectrum::operator +(const Spectrum &v) const {
    return {r + v.r, g + v.g, b + v.b};
}
Spectrum Spectrum::operator -(const Spectrum &v) const {
    return {r - v.r, g - v.g, b - v.b};
}
Spectrum Spectrum::operator *(const Spectrum &v) const {
    return {r * v.r, g * v.g, b * v.b};
}
Spectrum Spectrum::operator *(double k) const {
    return {r * k, g * k, b * k};
}
Spectrum Spectrum::operator /(double v) const {
    if (v == 0) return Spectrum(0);
    return {r / v, g / v, b / v};
}
Spectrum Spectrum::operator ^(double v) const {
    return {pow(r, v), pow(g, v), pow(b, v)};
}

int Spectrum::rgb256(double white, double gamma) const {
    int _r = static_cast<int>(min(pow(r, 1 / gamma) / white, 1.) * 255),
        _g = static_cast<int>(min(pow(g, 1 / gamma) / white, 1.) * 255),
        _b = static_cast<int>(min(pow(b, 1 / gamma) / white, 1.) * 255);
    return (_r * 256 + _g) * 256 + _b;
}

double Spectrum::norm() const {
    return sqrt(r * r + g * g + b * b);
}

Spectrum &Spectrum::operator +=(const Spectrum &v) { return *this = *this + v; }
Spectrum &Spectrum::operator -=(const Spectrum &v) { return *this = *this - v; }
Spectrum &Spectrum::operator *=(double f) { return *this = *this * f; }
Spectrum &Spectrum::operator /=(double f) { return *this = *this / f; }

ostream &operator <<(ostream &s, Spectrum t) {
    s << "(" << t.r << "," << t.g << "," << t.b << ")";
    return s;
}

Spectrum rgb256(double l, double gamma) {
    return Spectrum(pow(l / 255, gamma));
}
Spectrum rgb256(double r, double g, double b, double gamma) {
    return {pow(r / 255, gamma), pow(g / 255, gamma), pow(b / 255, gamma)};
}
Spectrum rgb(double l, double gamma) {
    return Spectrum(pow(l, gamma));
}
Spectrum rgb(double r, double g, double b, double gamma) {
    return {pow(r, gamma), pow(g, gamma), pow(b, gamma)};
}

#endif
#endif /* spectrum_hpp */
