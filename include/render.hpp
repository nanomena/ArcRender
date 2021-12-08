#ifndef render_hpp
#define render_hpp

#include <utility>

#include "utils.hpp"
#include "image.hpp"
#include "scene.hpp"

class Render {
    virtual void step(int idx) {
        throw invalid_argument("NotImplementedError");
    }
    virtual void build_graph() {}

protected:
    int length;
    shared_ptr<Image> image;
    shared_ptr<Scene> scene;

public:
    Render(const shared_ptr<Image>& image, const shared_ptr<Scene> &scene);
    void epoch(int cluster = 1);
    void step_one(int idx, int cluster = 1);
};

#ifdef ARC_IMPLEMENTATION

Render::Render(const shared_ptr<Image>& image, const shared_ptr<Scene> &scene) : image(image), scene(scene) {
    length = image->epoch();
}

void Render::epoch(int cluster) {
    build_graph();
//#pragma omp parallel for
    for (int i = 0; i < length; ++i)
        for (int j = 0; j < cluster; ++j)
            step(i);
}

void Render::step_one(int idx, int cluster) {
    build_graph();
//#pragma omp parallel for
    for (int j = 0; j < cluster; ++j)
        step(idx);
}

#endif
#endif /* render_hpp */
