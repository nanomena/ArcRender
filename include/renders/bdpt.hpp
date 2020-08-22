#ifndef renders_bdpt_hpp
#define renders_bdpt_hpp

#include "../render.hpp"

class BidirectionalPathTracer : public Render
{
    vector<vector<tuple<Vec3, Ray, Spectrum>>> photons;
    int trace_limit;
    double trace_eps;

    void build_graph() override;
    void step(int idx) override;
public:
    BidirectionalPathTracer(
        shared_ptr<oBuffer> image_, shared_ptr<Scene> scene_, int trace_limit_ = 6, double trace_eps_ = 1e-4
    );
};

#ifdef ARC_IMPLEMENTATION

BidirectionalPathTracer::BidirectionalPathTracer(
    shared_ptr<oBuffer> image_, shared_ptr<Scene> scene_, int trace_limit_, double trace_eps_
) : Render(std::move(image_), std::move(scene_)), trace_limit(trace_limit_), trace_eps(trace_eps_) {}


void BidirectionalPathTracer::build_graph()
{
    photons.resize(trace_limit);
    for (int cnt = 0; cnt < trace_limit; ++cnt)
        photons[cnt].clear();

    for (int i = 0; i < length; ++i)
    {
        auto &light = scene->Lights[rand() % scene->Lights.size()];
        Spectrum mul;
        Ray now;
        double pdf;
        Spectrum spectrum;
        light->sample_StV(now, pdf);
        light->evaluate_StV(now, spectrum);
        mul = spectrum / pdf;

        for (int cnt = 0; cnt < trace_limit; ++cnt)
        {
            shared_ptr<Object> object;
            Vec3 intersect;
            scene->inter(now, object, intersect);
            photons[cnt].emplace_back(intersect, now, mul);

//            if (cnt == 0) cerr << intersect << " " << now << " " << mul << endl;

            Ray next;
            object->sample_LtV(now, next, pdf);
            object->evaluate_LtV(now, next, spectrum);
            mul = mul * (spectrum / pdf);
            if (mul.norm() < trace_eps) break;
            now = next;
        }
    }
}

void BidirectionalPathTracer::step(int idx)
{
    Ray now = image->sample(idx);
    Spectrum mul(1);
    vector<vector<Spectrum>> sum;
    sum.resize(trace_limit);
    for (int i = 0; i < trace_limit; ++i) sum[i].resize(trace_limit);

    for (int cnt = 0; cnt < trace_limit; ++cnt)
    {
        shared_ptr<Object> object;
        Spectrum spectrum;
        Vec3 intersect;
        scene->inter(now, object, intersect);
        Ray next;
        double pdf;

        object->evaluate_VtS(now, spectrum);
        sum[cnt][0] = sum[cnt][0] + mul * spectrum;

        for (const auto &light : scene->Lights)
        {
            light->sample_S(intersect, next, pdf);
            shared_ptr<Object> o;
            Vec3 pos;
            scene->inter(next, o, pos);
            if (o == light) // visible @TODO this visibility test can only work on convex surface, fix later
            {
                Spectrum local;
                light->evaluate_VtS(next, spectrum);
                object->evaluate_VtL(now, next, local);
                sum[cnt][1] = sum[cnt][1] + mul * (spectrum * local / pdf);
            }
        }

        for (int rev = 0; cnt + rev + 2 < trace_limit; ++rev)
        {
            for (int i = 0; i < scene->Lights.size(); ++ i) // v = i + 1, l = j + 2
            {
                auto &photon = photons[rev][rand() % photons[rev].size()];
                Vec3 pho;
                Ray light;
                tie(pho, light, spectrum) = photon;
                Ray link(intersect, pho - intersect), link_b(pho, intersect - pho);
                shared_ptr<Object> dest;
                Vec3 pos;
                scene->inter(link, dest, pos);
                if ((pos - pho).norm() < EPS && dest != object) // visible
                {
                    Spectrum local, remote;
                    object->evaluate_VtL(now, link, local);
                    dest->evaluate_LtV(light, link_b, remote);
                    sum[cnt][rev + 2] = sum[cnt][rev + 2]
                        + mul * (spectrum * remote * local) / (pho - intersect).norm2();
                }
            }
        }

        object->sample_VtL(now, next, pdf);
        object->evaluate_VtL(now, next, spectrum);
        mul = mul * (spectrum / pdf);
        if (mul.norm() < trace_eps) break;
        now = next;
    }
    Spectrum total;
    for (int i = 0; i < trace_limit; ++ i)
        for (int j = 0; i + j < trace_limit; ++ j)
            total = total + sum[i][j] / (i + j + 1);
    image->draw(idx, total, 1);
}

#endif
#endif /* renders_bdpt_hpp */
