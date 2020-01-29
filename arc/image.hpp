#ifndef IMAGE
#define IMAGE
#include <string>
#include <fstream>
#include <iostream>
using namespace std;
#include "photon.hpp"
#include "camera.hpp"

class Image
{
    int width, height, length;
    shared_ptr<Camera> camera;
    vector<Color> color;
    vector<double> weight;

public:
    Image (int _width, int _height, shared_ptr<Camera> _camera)
    {
        width = _width, height = _height;
        camera = _camera;
        length = width * height;
        color.resize(length);
        weight.resize(length);
    }
    int epoch() { return length; } const
    Ray sample(int idx) const
    {
        int x = idx % width, y = idx / width;
        double dx = (x + RD.rand()) / width - 0.5, dy = (y + RD.rand()) / height - 0.5;
        $ << dx << " " << dy << endl;
        return camera->apply(dx, dy);
    }
    void draw(int idx, Color c, double w = 1)
    {
        color[idx] = color[idx] + c * w;
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
            auto _c = (color[i] / weight[i]).rgb888(white, gamma);
            out << char(_c / 65536) << char(_c / 256 % 256) << char(_c % 256);
        }
        out.close();
    }
};

#endif
