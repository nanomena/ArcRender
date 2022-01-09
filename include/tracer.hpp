#ifndef tracer_hpp
#define tracer_hpp

#include "utils.hpp"
#include "camera.hpp"
#include "spectrum.hpp"
#include "vecmath.hpp"
#include "sampler.hpp"
#include "scene.hpp"

#define MaxThreads 512

class Tracer {
public:
    Tracer(int width, int height, const Scene *scene);
    ~Tracer();

    virtual void epoch() = 0;

    void savePNG(const char *path, double white = 1, double gamma = 2.2) const;

protected:
    void sampleCamera(Ray &ray, int idx, Sampler &RNG) const;
    int getCameraIdx(const Vec2 &t) const;

    int width, height, length;
    Spectrum *spectrum;
    double *number;

    const Scene *scene;
};

#ifdef ARC_IMPLEMENTATION

Tracer::Tracer(
    int width, int height, const Scene *scene
) : width(width), height(height), scene(scene), length(width * height) {
    spectrum = new Spectrum[length];
    for (int i = 0; i < length; ++i) spectrum[i] = Spectrum(0);
    number = new double[length];
    for (int i = 0; i < length; ++i) number[i] = 0;
}

Tracer::~Tracer() {
    delete[] spectrum;
    delete[] number;
}

void Tracer::sampleCamera(Ray &ray, int idx, Sampler &RNG) const {
    int x = idx % width, y = idx / width;
    Vec2 t = RNG.pixel(Vec2(x, y), width, height);
    scene->camera->sample(ray, t, RNG);
    assert(abs(ray.d.length() - 1) < EPS);
}

int Tracer::getCameraIdx(const Vec2 &t) const {;
    int x = int(round((t.x() + .5) * width)), y = int(round((t.y() + .5) * height));
    if ((x < 0) || (x >= width) || (y < 0) || (y >= height)) return -1;
    return x + y * width;
}

void Tracer::savePNG(const char *path, double white, double gamma) const {
    vector<unsigned char> buffer;
    buffer.resize(length * 3);
    for (int i = 0; i < length; ++i) {
        auto _c = (spectrum[i] / number[i]).rgb256(white, gamma);
        buffer[i * 3 + 0] = (unsigned char)(_c / 65536);
        buffer[i * 3 + 1] = (unsigned char)(_c / 256 % 256);
        buffer[i * 3 + 2] = (unsigned char)(_c % 256);
    }
    stbi_write_png(path, width, height, 3, &buffer.front(), width * 3);
}

#endif
#endif /* tracer_hpp */
