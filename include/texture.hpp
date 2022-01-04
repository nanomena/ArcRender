#ifndef texture_hpp
#define texture_hpp

#include "utils.hpp"
#include "spectrum.hpp"

class Texture {
public:
    explicit Texture(const string &filename);
    Spectrum operator [](const Vec3 &v) const;

private:
    Spectrum get(int x, int y) const;

    int width{}, height{};
    vector<Spectrum> spectrum;
};

class TextureMap {
public:
    explicit TextureMap(const Spectrum &color);
    TextureMap(const shared_ptr<Texture> &texture, const Spectrum &color, Trans3 T);
    Spectrum operator [](const Vec3 &v) const;

private:
    shared_ptr<Texture> texture;
    Trans3 T;
    Spectrum color;
};

#ifdef ARC_IMPLEMENTATION

Texture::Texture(const string &filename) {
    int n;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &n, 0);
    cerr << width << " " << height << endl;
    spectrum.resize(width * height);
    for (int i = 0; i < spectrum.size(); ++i) {
        switch (n) {
        case 1:spectrum[i] = rgb256(data[i * n + 0]);
            break;
        case 2: throw invalid_argument("NotImplementedError");
        case 3: case 4: spectrum[i] = rgb256(data[i * n + 0], data[i * n + 1], data[i * n + 2]);
            break;
        default: throw invalid_argument("NotImplementedError");
        }
    }
}

Spectrum Texture::operator [](const Vec3 &v) const {
    double x = max(0., min(1., v.x())) * width, y = max(0., min(1., 1 - v.y())) * height;
    int ix = lround(x), iy = lround(y);
    return (get(ix - 1, iy - 1) * (ix + .5 - x) + get(ix, iy - 1) * (x + .5 - ix)) * (iy + .5 - y)
        + (get(ix - 1, iy) * (ix + .5 - x) + get(ix, iy) * (x + .5 - ix)) * (y + .5 - iy);
}

Spectrum Texture::get(int x, int y) const {
    return spectrum[max(0, min(x, width - 1)) + max(0, min(y, height - 1)) * width];
}

TextureMap::TextureMap(const Spectrum &color) : texture(nullptr), color(color) {}
TextureMap::TextureMap(const shared_ptr<Texture> &texture, const Spectrum &color, Trans3 T)
    : texture(texture), color(color), T(T) {}

Spectrum TextureMap::operator [](const Vec3 &v) const {
    if (texture == nullptr)
        return color;
    else return color * (*texture)[T * v];
}

#endif
#endif /* texture_hpp */