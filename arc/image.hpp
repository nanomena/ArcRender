#ifndef IMAGE
#define IMAGE
#include <string>
#include <fstream>
#include <iostream>
using namespace std;
#include "sampler.hpp"
#include "photon.hpp"
#include "camera.hpp"

class Image
{
    int width, height, length;
    shared_ptr<Camera> camera;
    vector<Spectrum> spectrum;
    vector<double> weight;

public:
    Image (int _width, int _height, shared_ptr<Camera> _camera)
    {
        width = _width, height = _height;
        camera = _camera;
        length = width * height;
        spectrum.resize(length);
        weight.resize(length);
    }
    int epoch() { return length; } const
    Ray sample(int idx) const
    {
        int x = idx % width, y = idx / width;
        pair<double,double> v = RD.pixel(x, y, width, height);
        double dx = v.first, dy = v.second;
        $ << "image sample " << dx << " " << dy << endl;
        return camera->apply(dx, dy);
    }
    void draw(int idx, Spectrum c, double w = 1)
    {
        spectrum[idx] = spectrum[idx] + c * w;
        weight[idx] += w;
    }
    void save(const char* path, double white = 1, double gamma = 2.2) const
    {
        ofstream out(path);
        out << "P6" << endl;
        out << width << " " << height << endl;
        out << 255 << endl;
        for (int i = 0; i < length; ++ i)
        {
            if (spectrum[i].r < 0)
                cerr << i << " " << spectrum[i].r << " " << spectrum[i].g << " " << spectrum[i].b << endl;
            auto _c = (spectrum[i] / weight[i]).rgb888(white, gamma);
            out << char(_c / 65536) << char(_c / 256 % 256) << char(_c % 256);
        }
        out.close();
    }
};

#endif
