#ifndef image_hpp
#define image_hpp

#include "utils.hpp"
#include "camera.hpp"
#include "spectrum.hpp"
#include "vecmath.hpp"
#include "sampler.hpp"

class Image
{
    int width, height, length;
    shared_ptr<Camera> camera;
    vector<Spectrum> spectrum;
    vector<double> weight;

public:
    Image(int width_, int height_, shared_ptr<Camera> camera_);

    int epoch() const;
    Ray sample(int idx) const;
    Spectrum get(int idx) const;
    void draw(int idx, Spectrum c, double w = 1);
    void save(const char *path, double white = 1, double gamma = 2.2) const;
};

#ifdef ARC_IMPLEMENTATION

Image::Image(int width_, int height_, shared_ptr<Camera> camera_)
{
    width = width_;
    height = height_;
    camera = std::move(camera_);
    length = width * height;
    spectrum.resize(length);
    weight.resize(length);
}

int Image::epoch() const
{
    return length;
}
Ray Image::sample(int idx) const
{
    int x = idx % width, y = idx / width;
    Vec2 v = RD.pixel(Vec2(x, y), width, height);
    return camera->apply(v);
}
void Image::draw(int idx, Spectrum c, double w)
{
    spectrum[idx] = spectrum[idx] + c * w;
    weight[idx] += w;
}
Spectrum Image::get(int idx) const
{
    return spectrum[idx] / weight[idx];
}
void Image::save(const char *path, double white, double gamma) const
{
    vector<unsigned char> buffer;
    buffer.resize(length * 3);
    for (int i = 0; i < length; ++i)
    {
        auto _c = (spectrum[i] / weight[i]).rgb256(white, gamma);
        buffer[i * 3 + 0] = (unsigned char)(_c / 65536);
        buffer[i * 3 + 1] = (unsigned char)(_c / 256 % 256);
        buffer[i * 3 + 2] = (unsigned char)(_c % 256);
    }
    stbi_write_png(path, width, height, 3, &buffer.front(), width * 3);
}

#endif
#endif /* image_hpp */
