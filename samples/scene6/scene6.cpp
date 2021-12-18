#define ARC_IMPLEMENTATION
#include "arc.hpp"
#include <bits/stdc++.h>
using namespace std;

//#define DEBUG_FLAG
int main() {
    ios::sync_with_stdio(false);


    long long T0 = time(nullptr);


    shared_ptr<Medium> medium = make_shared<Transparent>(Spectrum(1, 1, 1));
    // shared_ptr<Medium> medium = make_shared<Scatter>(0.02, Spectrum(1, 1, 1), 4, 1);
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
        Vec3(0, 0, 1.5),
        Vec3(0.8, 0, 0),
        Vec3(0, 0.6, 0),
        0.6
    );
#endif
    shared_ptr<Scene> scene = make_shared<Scene>(camera, skybox, medium);

    scene->addObject(
        make_shared<Sphere>(
            make_shared<BiGGX>(2.5, 0.2), nullptr,
            make_shared<Transparent>(Spectrum(0.2, 1, 0.2)),
            medium,
            Vec3(-.5, -.69, -1), .3
        ),
        "ball1"
    );

    scene->addObject(
        make_shared<Sphere>(
            make_shared<BiGGX>(1.05, 0.2), nullptr,
            make_shared<Transparent>(Spectrum(0.2, 1, 0.2)),
            medium,
            Vec3(.5, -.69, -1), .3
        ),
        "ball2"
    );

    scene->addObject(
        make_shared<Flat>(
            make_shared<Lambert>(Spectrum(0)),
            make_shared<UniformLight>(Spectrum(10)),
            medium, medium,
            Vec3(-0.3, 0.99, -0.7),
            Vec3(-0.3, 0.99, -1.3),
            Vec3(0.3, 0.99, -1.3),
            Vec3(0.3, 0.99, -0.7)
        ), "light"
    );

    scene->addObject(
        make_shared<Flat>(
            make_shared<GGX>(rgb256(250, 250, 250), 0.8), nullptr,
            medium, medium,
            Vec3(-1, -1, -2),
            Vec3(-1, -1, 2),
            Vec3(1, -1, 2),
            Vec3(1, -1, -2)
        ), "down"
    );

    scene->addObject(
        make_shared<Flat>(
            make_shared<GGX>(rgb256(250, 250, 250), 0.8), nullptr,
            medium, medium,
            Vec3(-1, 1, 2),
            Vec3(-1, 1, -2),
            Vec3(1, 1, -2),
            Vec3(1, 1, 2)
        ), "up"
    );

    scene->addObject(
        make_shared<Flat>(
            make_shared<GGX>(rgb256(170, 170, 250), 0.8), nullptr,
            medium, medium,
            Vec3(1, -1, -2),
            Vec3(1, -1, 2),
            Vec3(1, 1, 2),
            Vec3(1, 1, -2)
        ), "right"
    );

    scene->addObject(
        make_shared<Flat>(
            make_shared<GGX>(rgb256(250, 170, 170), 0.8), nullptr,
            medium, medium,
            Vec3(-1, -1, 2),
            Vec3(-1, -1, -2),
            Vec3(-1, 1, -2),
            Vec3(-1, 1, 2)
        ), "left"
    );

    scene->addObject(
        make_shared<Flat>(
            make_shared<Lambert>(rgb256(250, 250, 250)), nullptr,
            medium, medium,
            Vec3(-1, 1, -2),
            Vec3(-1, -1, -2),
            Vec3(1, -1, -2),
            Vec3(1, 1, -2)
        ), "back"
    );

    scene->addObject(
        make_shared<Flat>(
            make_shared<Lambert>(rgb256(250, 250, 250)), nullptr,
            medium, medium,
            Vec3(-1, -1, 2),
            Vec3(-1, 1, 2),
            Vec3(1, 1, 2),
            Vec3(1, -1, 2)
        ),
        "front"
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
    shared_ptr<Tracer> tracer = make_shared<BidirectionalPathTracer>(2400, 1800, scene);
#endif
    char output[100];
    sprintf(output, "result.png");

    int epoch = 10000000;

    cerr << "[T + " << time(nullptr) - T0 << "] | target : " << epoch << endl;
    for (int i = 1; i <= epoch; ++i) {
        tracer->epoch(0.33);
//        tracer->epoch(1);
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
