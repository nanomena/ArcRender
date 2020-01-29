#ifndef PHOTON
#define PHOTON
#include <cmath>
#include <iostream>
#include <algorithm>
using namespace std;
#include "geo.hpp"

struct Color
{
    double r, g, b;
    Color (double l = 0) { r = g = b = l; }
    Color (double _r, double _g, double _b) { r = _r, g = _g, b = _b; }

    Color operator + (Color v) const { return Color(r + v.r, g + v.g, b + v.b); }
    Color operator * (Color v) const { return Color(r * v.r, g * v.g, b * v.b); }
    Color operator / (double v) const
    {
        if (v == 0) return Color(0);
        return Color(r / v, g / v, b / v);
    }
    Color operator ^ (double v) const { return Color(pow(r, v), pow(g, v), pow(b, v)); }
    double norm2() const { return r * r + g * g + b * b; }
    int rgb888(double white, double gamma = 2.2) const
    {
        int _r = min(pow(r, 1 / gamma) / white, (double)1.) * 255,
            _g = min(pow(g, 1 / gamma) / white, (double)1.) * 255,
            _b = min(pow(b, 1 / gamma) / white, (double)1.) * 255;
        return (_r * 256 + _g) * 256 + _b;
    }

    friend ostream& operator << (ostream &s, Color t)
    {
        s << "(" << t.r << "," << t.g << "," << t.b << ")";
        return s;
    }
};

Color Light(double l = 0, double gamma = 2.2)
{
    return Color(pow(l, gamma));
}
Color Light(double r, double g, double b, double gamma = 2.2)
{
    return Color(pow(r, gamma), pow(g, gamma), pow(b, gamma));
}


struct Photon
{
    char stat; Ray ray; Color color;
    Photon (char _stat, Ray _ray, Color _color = Color(1))
    {
        stat = _stat;
        ray = _ray;
        color = _color;
    }
};


#endif
