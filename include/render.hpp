#ifndef render_hpp
#define render_hpp

#include "utils.hpp"
#include "image.hpp"
#include "scene.hpp"

class Render
{
    virtual void step(int idx)
    {
        throw invalid_argument("NotImplementedError");
    }
    virtual void build_graph() {}

protected:
    int length;
    shared_ptr<Image> image;
    shared_ptr<Scene> scene;

public:
    Render(shared_ptr<Image> image_, shared_ptr<Scene> scene_);
    void epoch(int cluster = 1);
    void step_one(int idx, int cluster = 1);
};

#ifdef ARC_IMPLEMENTATION

Render::Render(shared_ptr<Image> image_, shared_ptr<Scene> scene_)
{
    image = std::move(image_);
    scene = std::move(scene_);
    length = image->epoch();
}

void Render::epoch(int cluster)
{
    build_graph();
#pragma omp parallel for
    for (int i = 0; i < length; ++i)
        for (int j = 0; j < cluster; ++j)
            step(i);
}

void Render::step_one(int idx, int cluster)
{
    build_graph();
#pragma omp parallel for
    for (int j = 0; j < cluster; ++j)
        step(idx);
}

#endif
#endif /* render_hpp */
