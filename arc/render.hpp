#ifndef render_hpp
#define render_hpp

#include "utils.hpp"
#include "obuffer.hpp"
#include "sence.hpp"
#include "photon.hpp"

class Render
{
    int trace_limit, diffuse_limit, length;
    double trace_eps;
    shared_ptr<oBuffer> image;
    shared_ptr<Sence> sence;

public:
    Render (shared_ptr<oBuffer> _image, shared_ptr<Sence> _sence,
        int _trace_mxcnt = 8, int _diffuse_mxcnt = 3, double _trace_eps = 1e-30);

    void step(int idx) const;
    void epoch(int cluster = 1) const;
};

#ifdef library

Render::Render (shared_ptr<oBuffer> _image, shared_ptr<Sence> _sence,
    int _trace_mxcnt, int _diffuse_mxcnt, double _trace_eps)
{
    image = _image;
    sence = _sence;
    trace_limit = _trace_mxcnt;
    diffuse_limit = _diffuse_mxcnt;
    trace_eps = _trace_eps;
    length = image->epoch();

    sence->build_graph();
}

void Render::step(int idx) const
{
    double weight = 1;
    Photon photon(image->sample(idx));
    int diffuse_cnt = 0, matched = 0;
    for (int trace_cnt = 0; (trace_cnt < trace_limit) && weight > trace_eps; ++ trace_cnt)
    {
        int type;
        $ << "tracing " << photon.ray << endl;
        weight *= sence->forward(photon, type);
        if (type == -1) {matched = 1; break;}
        if (type == 1)
        {
            if (diffuse_cnt < diffuse_limit)
                diffuse_cnt ++;
            else break;
        }
    }
    // $ << photon.spectrum << " " << weight << endl;
    if (!matched) photon.trans(Spectrum(0));
    image->draw(idx, photon.spectrum, weight);
}
void Render::epoch(int cluster) const
{
    #pragma omp parallel for
    for (int i = 0; i < length; ++ i)
        for (int j = 0; j < cluster; ++ j)
            step(i);
}

#endif
#endif /* render_hpp */
