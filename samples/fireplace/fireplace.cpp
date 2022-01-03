#define ARC_IMPLEMENTATION
#include "arc.hpp"
#include <bits/stdc++.h>
#include <filesystem>
using namespace std;

//#define DEBUG_FLAG
int main() {
    ios::sync_with_stdio(false);


    long long T0 = time(nullptr);


    shared_ptr<Medium> medium = make_shared<Transparent>(Spectrum(1, 1, 1));
//    shared_ptr<Medium> medium = make_shared<Scatter>(0.3, Spectrum(1, 1, 1));
    shared_ptr<Shape> skybox = make_shared<Sphere>(
        make_shared<Lambert>(Spectrum(0)),
        nullptr,
        nullptr, medium,
        Vec3(0, 0, 0), INF / 10, true
    );
    skybox->setIdentifier("skybox");


#ifdef DEBUG_FLAG
    shared_ptr<Camera> camera = make_shared<Actinometer>(
        Vec3(0, 0, 15),
        Vec3(3, 0, -10)
    );
#else
    shared_ptr<Camera> camera = make_shared<PerspectiveCamera>(
        Vec3(2.5, 1.5, -0.1),
        Vec3(0.8, 0, 0),
        Vec3(0, 0.6, 0),
        0.6
    );
#endif
    shared_ptr<Scene> scene = make_shared<Scene>(camera, skybox, medium);


    vector<shared_ptr<Shape>> objects;
    if (!LoadModel("models/fireplace_room.obj", "models/", Trans3(
        Vec3(0, 0, 0),
        Vec3(1, 0, 0),
        Vec3(0, 1, 0),
        Vec3(0, 0, 1)
    ), medium, scene)) return -1;

    scene->addObject(
        make_shared<Flat>(
            make_shared<Lambert>(Spectrum(0)),
            make_shared<UniformLight>(Spectrum(10)),
            medium, medium,
            Vec3(2, 2, -1),
            Vec3(2, 2, -2),
            Vec3(3, 2, -2),
            Vec3(3, 2, -1)
        ), "light"
    );

//
//
//
//    shared_ptr<Image> image = make_shared<Image>(800, 600, camera);
//    shared_ptr<Render> render = make_shared<LightSampledPathTracer>(image, scene);
//

#ifdef DEBUG_FLAG
    shared_ptr<Tracer> tracer = make_shared<BidirectionalPathTracer>(1, 1, scene);
#else
//    shared_ptr<Tracer> tracer = make_shared<StochasticProgressivePhotonMapping>(800, 600, scene);
    shared_ptr<Tracer> tracer = make_shared<BidirectionalPathTracer>(800, 600, scene);
#endif
    char output[100];
    sprintf(output, "samples/fireplace/result.png");

    int epoch = 1000;

    cerr << "[T + " << time(nullptr) - T0 << "] | target : " << epoch << endl;
    for (int i = 1; i <= epoch; ++i) {
        tracer->epoch();
#ifdef DEBUG_FLAG
        if (i % 10000 == 0) {
            cerr << "[T + " << ((double)clock() / CLOCKS_PER_SEC) << "] | epoch " << i << endl;
            tracer->color(0, true);
        }
#else
        if (i % 1 == 0) {
            cerr << "[T + " << time(nullptr) - T0 << "] | epoch " << i << endl;
            tracer->savePNG(output, 1);
        }
#endif
    }
}
