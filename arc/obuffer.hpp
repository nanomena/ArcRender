#ifndef obuffer_hpp
#define obuffer_hpp

#include "utils.hpp"
#include "camera.hpp"
#include "spectrum.hpp"
#include "geometry.hpp"
#include "sampler.hpp"

class oBuffer
{
    int width, height, length;
    shared_ptr<Camera> camera;
    vector<Spectrum> spectrum;
    vector<double> weight;

public:
    oBuffer (int _width, int _height, shared_ptr<Camera> _camera);

    int epoch() const;
    Ray sample(int idx) const;
    void draw(int idx, Spectrum c, double w = 1);
    void save(const char* path, double white = 1, double gamma = 2.2) const;
};

#ifdef library

oBuffer::oBuffer (int _width, int _height, shared_ptr<Camera> _camera)
{
    width = _width;
    height = _height;
    camera = _camera;
    length = width * height;
    spectrum.resize(length);
    weight.resize(length);
}

int oBuffer::epoch() const
{
    return length;
}
Ray oBuffer::sample(int idx) const
{
    int x = idx % width, y = idx / width;
    Vec2 v = RD.pixel(Vec2(x, y), width, height);
    return camera->apply(v);
}
void oBuffer::draw(int idx, Spectrum c, double w)
{
    spectrum[idx] = spectrum[idx] + c * w;
    weight[idx] += w;
}
void oBuffer::save(const char* path, double white, double gamma) const
{
    ofstream out(path);
    out << "P6" << endl;
    out << width << " " << height << endl;
    out << 255 << endl;
    for (int i = 0; i < length; ++ i)
    {
        auto _c = (spectrum[i] / weight[i]).rgb888(white, gamma);
        out << char(_c / 65536) << char(_c / 256 % 256) << char(_c % 256);
    }
    out.close();
}


#endif
#endif /* obuffer_hpp */
