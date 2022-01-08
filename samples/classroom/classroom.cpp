//#define ARC_IMPLEMENTATION
#include "arc.hpp"
#include <bits/stdc++.h>
using namespace std;

//#define DEBUG_FLAG
int main() {
    ios::sync_with_stdio(false);


    long long T0 = time(nullptr);


//    auto medium = new Transparent(Spectrum(1, 1, 1));
    auto medium = new Scatter(0.03, Spectrum(1, 1, 1));

    auto camera = new PerspectiveCamera(
        Vec3(3.4, 1.5, 3.95),
        Vec3(1, 0, -0.5).norm() * 0.8,
        Vec3(-0.10, 1, -0.20).norm() * 0.6,
        0.6
    );

//    auto camera = new PerspectiveCamera(
//        Vec3(-1.4, 1.5, 3.95),
//        Vec3(1, 0, 0.5).norm() * 0.8,
//        Vec3(0.10, 1, -0.20).norm() * 0.6,
//        0.6
//    );
    auto scene = new Scene(camera, rgb256(245, 251, 253), medium, 16);

    Model model("models/classroom.obj", "models/", Trans3(
        Vec3(0, 0, 0),
        Vec3(1, 0, 0),
        Vec3(0, 1, 0),
        Vec3(0, 0, 1)
    ), medium);
    scene->addObjects(model.get());

    scene->addObject(
        new Disc(
            new Lambert(Spectrum(0)),
            new SpotLight(rgb256(249, 236, 214) * 120, 0.05),
            medium, medium,
            Ray({8, 5, 2}, Vec3(-1, -.5, -.2).norm()),
            8
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

    auto tracer = new BidirectionalPathTracer(800, 600, scene);

    char output[100];
    sprintf(output, "samples/classroom/result.png");

    int epoch = 2000;

    cerr << "[T + " << time(nullptr) - T0 << "] | target : " << epoch << endl;
    for (int i = 1; i <= epoch; ++i) {
        tracer->epoch();
        cerr << "[T + " << time(nullptr) - T0 << "] | epoch " << i << endl;
        tracer->savePNG(output, 1);
    }
}
