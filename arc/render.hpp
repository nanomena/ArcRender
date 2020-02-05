#ifndef RENDER
#define RENDER
#define DEBUG 0
// #define double long double
#define $ if (DEBUG) cerr
#include "sence.hpp"
#include "image.hpp"
#include "photon.hpp"
#include "camera.hpp"
#include <thread>
using namespace std;


class Render
{
    int trace_limit, diffuse_limit, length;
    double trace_eps;
    shared_ptr<Image> image;
    shared_ptr<Sence> sence;

public:
    Render (shared_ptr<Image> _image, shared_ptr<Sence> _sence,
        int _trace_mxcnt = 8, int _diffuse_mxcnt = 3, double _trace_eps = 1e-30)
    {
        image = _image;
        sence = _sence;
        trace_limit = _trace_mxcnt;
        diffuse_limit = _diffuse_mxcnt;
        trace_eps = _trace_eps;
        length = image->epoch();

        sence->build_graph();
    }

    void step(int idx) const
    {
        double weight = 1;
        Photon photon(image->sample(idx));
        $ << "step : " << idx << endl;
        int diffuse_cnt = 0, matched = 0;
        for (int trace_cnt = 0; (trace_cnt < trace_limit) && weight > trace_eps; ++ trace_cnt)
        {
            $ << "tracing " << photon.ray << endl;
            int type;
            weight *= sence->forward(photon, type);
            if (type == -1) {matched = 1; break;}
            if (type == 1) if (diffuse_cnt < diffuse_limit) diffuse_cnt ++; else break;
        }
        $ << "finish tracing : " << matched << " " << weight << " " << photon.spectrum << endl;
        if (!matched) photon.trans(Spectrum(0));
        image->draw(idx, photon.spectrum, weight);
    }
    void epoch(int cluster = 1) const
    {
        #pragma omp parallel for
        for (int i = 0; i < length; ++ i)
            for (int j = 0; j < cluster; ++ j)
                step(i);
    }
};

void run(int thread_id, shared_ptr<Render> render, int epoch)
{
    for (int i = 0; i < epoch; ++ i)
    {
        render->epoch();
        if (thread_id == 0)
            cerr << "epoch " << i << endl;
    }
}

#endif
