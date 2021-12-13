#ifndef tracer_hpp
#define tracer_hpp

#include "utils.hpp"
#include "camera.hpp"
#include "spectrum.hpp"
#include "vecmath.hpp"
#include "sampler.hpp"
#include "counter.hpp"

class Tracer {
public:
    Tracer(int width, int height, const shared_ptr<Scene> &scene, int routine = 1, int pass = 1);

    virtual void initCache();
    virtual void preSample(int idx);
    virtual void sample(int idx);
    void epoch(int preCnt);

    Spectrum color(int idx, int debug = 0) const;
    void savePNG(const char *path, double white = 1, double gamma = 2.2) const;

protected:
    Ray sampleCamera(int idx) const;
    void add(int idx, Spectrum color, int routine = 0, int pass = 0);

    int width, height, length;
    vector<vector<Counter>> counters;

    shared_ptr<Scene> scene;
};

#ifdef ARC_IMPLEMENTATION

Tracer::Tracer(
    int width, int height, const shared_ptr<Scene> &scene, int routine, int pass
) : width(width), height(height), scene(scene) {
    length = width * height;
    counters.resize(length);
    for (int i = 0; i < length; ++i)
        for (int j = 0; j < routine; ++j)
            counters[i].emplace_back(pass);
}

void Tracer::initCache() {}

void Tracer::preSample(int idx) {
    throw invalid_argument("NotImplementedError");
}
void Tracer::sample(int idx) {
    throw invalid_argument("NotImplementedError");
}
void Tracer::epoch(int preCnt) {
    initCache();
    for (int i = 0; i < preCnt; ++i) preSample(i);
    for (int i = 0; i < length; ++i) sample(i);
}

Ray Tracer::sampleCamera(int idx) const {
    int x = idx % width, y = idx / width;
    Vec2 v = RD.pixel(Vec2(x, y), width, height);
    return scene->camera->apply(v);
}

void Tracer::add(int idx, Spectrum color, int routine, int pass) {
    counters[idx][routine].add(color, pass);
}

Spectrum Tracer::color(int idx, int debug) const {
    Spectrum tot;
    for (const auto &c: counters[idx]) {
        tot = tot + c.color();
        if (debug) cerr << c << endl;
    }
    return tot;
}

void Tracer::savePNG(const char *path, double white, double gamma) const {
    vector<unsigned char> buffer;
    buffer.resize(length * 3);
    for (int i = 0; i < length; ++i) {
        auto _c = color(i).rgb256(white, gamma);
        buffer[i * 3 + 0] = (unsigned char)(_c / 65536);
        buffer[i * 3 + 1] = (unsigned char)(_c / 256 % 256);
        buffer[i * 3 + 2] = (unsigned char)(_c % 256);
    }
    stbi_write_png(path, width, height, 3, &buffer.front(), width * 3);
}

#endif
#endif /* tracer_hpp */
