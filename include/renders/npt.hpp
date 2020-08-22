#ifndef renders_npt_hpp
#define renders_npt_hpp

#include "../render.hpp"

class NaivePathTracer : public Render
{
    int trace_limit;
    double trace_eps;

    void step(int idx) override;
public:
    NaivePathTracer(
        shared_ptr<oBuffer> image_, shared_ptr<Scene> scene_, int trace_limit_ = 7, double trace_eps_ = 1e-4
    );
};

#ifdef ARC_IMPLEMENTATION

NaivePathTracer::NaivePathTracer(
    shared_ptr<oBuffer> image_, shared_ptr<Scene> scene_, int trace_limit_, double trace_eps_
) : Render(std::move(image_), std::move(scene_)), trace_limit(trace_limit_), trace_eps(trace_eps_) {}

void NaivePathTracer::step(int idx)
{
    Ray now = image->sample(idx);
    Spectrum mul(1);
    vector<Spectrum> sum;
    sum.resize(trace_limit);

    for (int cnt = 0; cnt < trace_limit; ++cnt)
    {
        shared_ptr<Object> object;
        Spectrum spectrum;
        Vec3 intersect;
        scene->inter(now, object, intersect);
        object->evaluate_VtS(now, spectrum);
        sum[cnt] = sum[cnt] + mul * spectrum;

        Ray next;
        double pdf;
        object->sample_VtL(now, next, pdf);
        object->evaluate_VtL(now, next, spectrum);
        mul = mul * (spectrum / pdf);
        if (mul.norm() < trace_eps) break;
        now = next;
    }
    Spectrum total;
    for (int cnt = 0; cnt < trace_limit; ++cnt)
        total = total + sum[cnt];
    image->draw(idx, total, 1);
}


#endif
#endif /* renders_nrt_hpp */
