//#define ARC_IMPLEMENTATION
#include "arc.hpp"
#include <bits/stdc++.h>
using namespace std;

//#define DEBUG_FLAG
int main() {
    ios::sync_with_stdio(false);


    long long T0 = time(nullptr);


    shared_ptr<Medium> medium = make_shared<Transparent>(Spectrum(1, 1, 1));
//    shared_ptr<Medium> medium = make_shared<Scatter>(0.025, Spectrum(1, 1, 1));

    shared_ptr<Camera> camera = make_shared<PerspectiveCamera>(
        Vec3(2.95, 1.4, 3.45),
        Vec3(1, 0, -0.5).norm() * 0.8,
        Vec3(-0.10, 1, -0.20).norm() * 0.6,
        0.6
//        Vec3(-0.3, 1.2, 3.95),
//        Vec3(1, 0, 0.6).norm() * 0.8,
//        Vec3(0.24, 1, -0.40).norm() * 0.6,
//        0.6
    );
    shared_ptr<Scene> scene = make_shared<Scene>(camera, rgb256(245, 251, 253), medium);


    vector<shared_ptr<Shape>> objects;
    if (!LoadModel("models/class3_obj.obj", "models/", Trans3(
        Vec3(0, 0, 0),
        Vec3(1, 0, 0),
        Vec3(0, 1, 0),
        Vec3(0, 0, 1)
    ), medium, scene)) return -1;

    scene->addObject(
        make_shared<Disc>(
            make_shared<Lambert>(Spectrum(0)),
            make_shared<SpotLight>(rgb256(242, 230, 209) * 25, 0.2),
            medium, medium,
            Ray({10, 5, 2}, Vec3(-1, -.5, -.2).norm()),
            5
        ), "right"
    );

//    scene->addObject(
//        make_shared<Sphere>(
//            make_shared<BiGGX>(2.5, 0.2), nullptr,
//            make_shared<Transparent>(Spectrum(0.2, 1, 0.2)),
//            medium,
//            Vec3(2.7, 0.9, 3.5), .02
//        ),
//        "ball1"
//    );

//    shared_ptr<Tracer> tracer = make_shared<BidirectionalPathTracer>(400, 300, scene);
    shared_ptr<Tracer> tracer = make_shared<BidirectionalPathTracer>(2400, 1800, scene);

    char output[100];
    sprintf(output, "samples/classroom/result.png");

    int epoch = 1000;

    cerr << "[T + " << time(nullptr) - T0 << "] | target : " << epoch << endl;
    for (int i = 1; i <= epoch; ++i) {
        tracer->epoch();
        cerr << "[T + " << time(nullptr) - T0 << "] | epoch " << i << endl;
        tracer->savePNG(output, 1);
    }
}
