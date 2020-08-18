#ifndef render_hpp
#define render_hpp

#include <utility>

#include "utils.hpp"
#include "obuffer.hpp"
#include "scene.hpp"
#include "photon.hpp"

class Render
{
    virtual void step(int idx) const
    {
        throw invalid_argument("NotImplementedError");
    }

protected:
    int length;
    shared_ptr<oBuffer> image;
    shared_ptr<Scene> scene;

public:
    Render(shared_ptr<oBuffer> image_, shared_ptr<Scene> scene_);
    void epoch(int cluster = 1) const;
};

class NaivePathTracer : public Render
{
    int trace_limit;
    double trace_eps;

    void step(int idx) const override;
public:
    NaivePathTracer(
        shared_ptr<oBuffer> image_, shared_ptr<Scene> scene_, int trace_limit_ = 4, double trace_eps_ = 1e-4
    );
};
#ifdef ARC_IMPLEMENTATION

Render::Render(shared_ptr<oBuffer> image_, shared_ptr<Scene> scene_)
{
    image = std::move(image_);
    scene = std::move(scene_);
    length = image->epoch();
}

NaivePathTracer::NaivePathTracer(
    shared_ptr<oBuffer> image_, shared_ptr<Scene> scene_, int trace_limit_, double trace_eps_
) : Render(image_, scene_), trace_limit(trace_limit_), trace_eps(trace_eps_) {}

void NaivePathTracer::step(int idx) const
{
    Ray now = image->sample(idx);
    Spectrum mul(1), sum(0);

    for (int cnt = 0; cnt < trace_limit; ++cnt)
    {
        shared_ptr<Object> object;
        Spectrum spectrum;
        Vec3 intersect;
        scene->inter(now, object, intersect);
        object->evaluate_VtS(now, spectrum);
        sum = sum + mul * spectrum;

        Ray next;
        double pdf;
        object->sample_VtL(now, next, pdf);
        object->evaluate_VtL(now, next, spectrum);
        mul = mul * (spectrum / pdf);
        if (spectrum.norm() < trace_eps) break;

        now = next;
    }
    image->draw(idx, sum, 1);
}
void Render::epoch(int cluster) const
{
#pragma omp parallel for
    for (int i = 0; i < length; ++i)
        for (int j = 0; j < cluster; ++j)
            step(i);
}

#endif
#endif /* render_hpp */
