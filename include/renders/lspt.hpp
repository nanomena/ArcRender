#ifndef renders_lspt_hpp
#define renders_lspt_hpp

#include "../render.hpp"

class LightSampledPathTracer : public Render {
    int trace_limit;
    double trace_eps;

    void step(int idx) override;
public:
    LightSampledPathTracer(
        const shared_ptr<Image>& image, const shared_ptr<Scene> &scene, int trace_limit = 6, double trace_eps = 1e-4
    );
};

#ifdef ARC_IMPLEMENTATION

LightSampledPathTracer::LightSampledPathTracer(
    const shared_ptr<Image>& image, const shared_ptr<Scene> &scene, int trace_limit, double trace_eps
) : Render(image, scene), trace_limit(trace_limit), trace_eps(trace_eps) {}

void LightSampledPathTracer::step(int idx) {
    Ray now = image->sample(idx);
    Spectrum mul(1);
    vector<Spectrum> sum;
    sum.resize(trace_limit);

    for (int cnt = 0; cnt + 1 < trace_limit; ++cnt) {
        shared_ptr<Object> object;
        Spectrum spectrum;
        double t;
        scene->intersect(now, object, t);
        Vec3 intersect = now.o + t * now.d;
        if (cnt == 0) // direct light
        {
            object->evaluate_VtS(now, spectrum);
            sum[cnt] = sum[cnt] + mul * spectrum;
        }

        Ray next;
        double pdf;

        for (const auto &light: scene->Lights) {
            light->sample_S(intersect, next, pdf);
//            next = Ray(intersect, RD.sphere()); pdf = 1 / (4 * pi);

            shared_ptr<Object> o;
            double tt;
            scene->intersect(next, o, tt);
            if (o == light) // visible @TODO this visibility test can only work on convex surface, fix later
            {
                Spectrum local;
                light->evaluate_VtS(next, spectrum);
                object->evaluate_VtL(now, next, local);
                sum[cnt + 1] = sum[cnt + 1] + mul * (spectrum * local / pdf);
            }
        }

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
#endif /* renders_lspt_hpp */
