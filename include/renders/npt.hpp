#ifndef renders_npt_hpp
#define renders_npt_hpp

#include "../render.hpp"

class NaivePathTracer : public Render {
    int trace_limit;
    double trace_eps;

    void step(int idx) override;
public:
    NaivePathTracer(
        const shared_ptr<Image>& image, const shared_ptr<Scene> &scene, int trace_limit = 7, double trace_eps = 1e-4
    );
};

#ifdef ARC_IMPLEMENTATION

NaivePathTracer::NaivePathTracer(
    const shared_ptr<Image>& image, const shared_ptr<Scene> &scene, int trace_limit, double trace_eps
) : Render(image, scene), trace_limit(trace_limit), trace_eps(trace_eps) {}

void NaivePathTracer::step(int idx) {
    Ray now = image->sample(idx);
    Spectrum mul(1);
    vector<Spectrum> sum;
    sum.resize(trace_limit);

    for (int cnt = 0; cnt < trace_limit; ++cnt) {
        shared_ptr<Object> object;
        Spectrum spectrum;
        double t;
        scene->intersect(now, object, t);

//        cerr << object->name << endl;
//        cerr << now.o << " " << now.d << endl;

        object->evaluate_VtS(now, spectrum);


//        cerr << spectrum << endl;
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
#endif /* renders_npt_hpp */
