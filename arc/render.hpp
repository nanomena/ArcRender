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
    int trace_limit, length;
    double trace_eps;
    shared_ptr<Image> image;
    shared_ptr<Sence> sence;

public:
    Render (shared_ptr<Image> _image, shared_ptr<Sence> _sence,
        int _trace_mxcnt = 6, double _trace_eps = 1e-3)
    {
        image = _image;
        sence = _sence;
        trace_limit = _trace_mxcnt;
        trace_eps = _trace_eps;
        length = image->epoch();
    }

    void step(int idx) const
    {
        Photon photon(1, image->sample(idx));
        $ << idx << " : tracing " << photon.ray << endl;
        for (int cnt = 0; (cnt < trace_limit) && !(photon.stat & 2); ++ cnt)
            photon = sence->forward(photon);
        $ << int(photon.stat) << " " << photon.color << endl;
        if (!(photon.stat & 2))
            photon = Photon(3, Ray(), Color(0));
        image->draw(idx, photon.color);
    }
    void epoch() const
    {
        for (int i = 0; i < length; ++ i) step(i);
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
