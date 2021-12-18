#ifndef tracer_hpp
#define tracer_hpp

#include "utils.hpp"
#include "camera.hpp"
#include "spectrum.hpp"
#include "vecmath.hpp"
#include "sampler.hpp"
#include "counter.hpp"
#include "scene.hpp"

#define MaxThreads 512

class Tracer {
public:
    Tracer(int width, int height, const shared_ptr<Scene> &scene);

    virtual void initGraph(int preCnt);
    virtual void buildGraph();
    virtual void preSample(int idx, double mul, Sampler &RNG) = 0;
    virtual void sample(int idx, Sampler &RNG) = 0;
    void epoch(double mul);

    void savePNG(const char *path, double white = 1, double gamma = 2.2) const;

protected:
    Ray sampleCamera(int idx, double &pdf, Sampler &RNG) const;
    int getCameraIdx(const Vec2 &t) const;

    int width, height, length;
    vector<Spectrum> C;
    vector<double> W;

    vector<Sampler> RNGs;
    shared_ptr<Scene> scene;
};

#ifdef ARC_IMPLEMENTATION

Tracer::Tracer(
    int width, int height, const shared_ptr<Scene> &scene
) : width(width), height(height), scene(scene) {
    length = width * height;
    C.resize(length);
    W.resize(length);
}

void Tracer::initGraph(int preCnt) {}

void Tracer::buildGraph() {}

void Tracer::epoch(double mul) {
    int preCnt = max(int(length * mul), 1);

    if (RNGs.empty()) {
        for (int i = 0; i < MaxThreads; ++ i) RNGs.emplace_back();
    }

    initGraph(preCnt);
#pragma omp parallel for schedule(dynamic, 1)
    for (int i = 0; i < preCnt; ++i) preSample(i, mul, RNGs[i % MaxThreads]);
    buildGraph();
#pragma omp parallel for schedule(dynamic, 1)
    for (int i = 0; i < length; ++i) sample(i, RNGs[i % MaxThreads]);
}

Ray Tracer::sampleCamera(int idx, double &pdf, Sampler &RNG) const {
    int x = idx % width, y = idx / width;
    Vec2 t = RNG.pixel(Vec2(x, y), width, height);
    return scene->camera->sample(t, pdf, RNG);
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
        auto _c = (C[i] / W[i]).rgb256(white, gamma);
        buffer[i * 3 + 0] = (unsigned char)(_c / 65536);
        buffer[i * 3 + 1] = (unsigned char)(_c / 256 % 256);
        buffer[i * 3 + 2] = (unsigned char)(_c % 256);
    }
    stbi_write_png(path, width, height, 3, &buffer.front(), width * 3);
}

#endif
#endif /* tracer_hpp */
