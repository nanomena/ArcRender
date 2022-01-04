#define ARC_IMPLEMENTATION
#include "arc.hpp"
#include <bits/stdc++.h>
using namespace std;

//#define DEBUG_FLAG
int main() {
    ios::sync_with_stdio(false);


    long long T0 = time(nullptr);


    shared_ptr<Medium> medium = make_shared<Transparent>(Spectrum(1, 1, 1));

    shared_ptr<Camera> camera = make_shared<PerspectiveCamera>(
        Vec3(0, 0, 1.5),
        Vec3(0.8, 0, 0),
        Vec3(0, 0.6, 0),
        0.6
    );
    shared_ptr<Scene> scene = make_shared<Scene>(camera, Spectrum(0), medium);


    vector<shared_ptr<Shape>> objects;
    if (!LoadModel("models/teapot.obj", "models/", Trans3(
        Vec3(0, -.9, -1),
        Vec3(0.24, 0, 0),
        Vec3(0, 0.24, 0),
        Vec3(0, 0, 0.24)
    ), medium, scene)) return -1;

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

    shared_ptr<Tracer> tracer = make_shared<BidirectionalPathTracer>(1600, 1200, scene);

    char output[100];
    sprintf(output, "samples/scene8/result.png");

    int epoch = 1000;

    cerr << "[T + " << time(nullptr) - T0 << "] | target : " << epoch << endl;
    for (int i = 1; i <= epoch; ++i) {
        tracer->epoch();
        cerr << "[T + " << time(nullptr) - T0 << "] | epoch " << i << endl;
        tracer->savePNG(output, 1);
    }
}
