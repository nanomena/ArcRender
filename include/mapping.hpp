#ifndef mapping_hpp
#define mapping_hpp

#include "utils.hpp"

struct Pixel
{
    double r, g, b, a;
    Pixel (double grey = 0, double _a = 0);
    Pixel (double _r, double _g, double _b, double _a = 0);

    Pixel operator + (const Pixel &t) const;
    Pixel operator - (const Pixel &t) const;
    Pixel operator * (double t) const;
    Pixel operator / (double t) const;

    friend ostream& operator << (ostream &s, Pixel t);
};

struct Mapping
{
    vector<Pixel> image;
    int width, height;

    Mapping ();
    Mapping (string filename);

    Pixel getp(int x, int y) const;
    Pixel get(double x, double y) const;
};

#ifdef ARC_IMPLEMENTATION

Pixel::Pixel(double grey, double _a) { r = g = b = grey; a = _a; }
Pixel::Pixel(double _r, double _g, double _b, double _a) { r = _r, g = _g, b = _b, a = _a; }
Pixel Pixel::operator + (const Pixel &t) const
{
    return Pixel(r + t.r, g + t.g, b + t.b, a + t.a);
}
Pixel Pixel::operator - (const Pixel &t) const
{
    return Pixel(r - t.r, g - t.g, b - t.b, a - t.a);
}
Pixel Pixel::operator * (double t) const
{
    return Pixel(r * t, g * t, b * t, a * t);
}
Pixel Pixel::operator / (double t) const
{
    return Pixel(r / t, g / t, b / t, a / t);
}
ostream& operator << (ostream &s, Pixel t)
{
    s << "(" << t.r << "," << t.g << "," << t.b << "," << t.a << ")";
    return s;
}

Mapping::Mapping () {}
Mapping::Mapping (string filename)
{
    int n;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &n, 0);
    image.resize(width * height);
    for (int i = 0; i < image.size(); ++ i)
    {
        switch (n)
        {
        case 1:
            image[i] = Pixel(data[i * n + 0] / 255.);
            break;
        case 2:
            image[i] = Pixel(data[i * n + 0] / 255., 1 - data[i * n + 1] / 255.);
            break;
        case 3:
            image[i] = Pixel(data[i * n + 0] / 255., data[i * n + 1] / 255.,
                data[i * n + 2] / 255.);
            break;
        case 4:
            image[i] = Pixel(data[i * n + 0] / 255., data[i * n + 1] / 255.,
                data[i * n + 2] / 255., 1 - data[i * n + 3] / 255.);
            break;
        }
    }
}
Pixel Mapping::getp(int x, int y) const
{
    return image[min(x, width - 1) + min(y, height - 1) * width];
}
Pixel Mapping::get(double px, double py) const
{
    py = 1 - py;
    if (px < 0) px = 0; if (px >= 1) px = 1;
    if (py < 0) py = 0; if (py >= 1) py = 1;
    double x = px * (width - 1), y = py * (height - 1);
    int nx = x, ny = y;
    Pixel w = (getp(nx, ny) * (nx + 1 - x) + getp(nx + 1, ny) * (x - nx)) * (ny + 1 - y)
        + (getp(nx, ny + 1) * (nx + 1 - x) + getp(nx + 1, ny + 1) * (x - nx)) * (y - ny);
    return w;
}

#endif
#endif /* mapping_hpp */
