#include "arc.hpp"
#include <bits/stdc++.h>
using namespace std;

//#define DEBUG_FLAG
int main() {
    ios::sync_with_stdio(false);


    long long T0 = time(nullptr);


    auto medium = new Transparent(Spectrum(1, 1, 1));

    auto camera = new PerspectiveCamera(
        Vec3(-.3, -.4, .3),
        Vec3(0.8, 0, 0),
        Vec3(0, 0.6 * sqrt(3. / 5), -0.6 * sqrt(2. / 5)),
        0.6,
        1.2, 0.002, 0.1
    );
    auto scene = new Scene(camera, rgb256(245, 251, 253), medium, 50);

    Model model2("models/glass_0.obj", "models/", Trans3(
        Vec3(0, -0.4, -1),
        Vec3(2, 0, 0),
        Vec3(0, 2, 0),
        Vec3(0, 0, 2)
    ), medium, true);
    scene->addObjects(model2.get());


//    scene->addObject(
//        new Flat(
//            new Lambert(Spectrum(0)),
//            new UniformLight(Spectrum(2)),
//            medium, medium,
//            Vec3(-0.3, 0.99, -0.7),
//            Vec3(-0.3, 0.99, -1.3),
//            Vec3(0.3, 0.99, -1.3),
//            Vec3(0.3, 0.99, -0.7)
//        ), "light"
//    );

    scene->addObject(
        new Disc(
            new Lambert(Spectrum(0)),
            new SpotLight(Spectrum(150), 0.02),
            medium, medium,
            {{10, 19, -11}, Vec3(-1, -2, 1).norm()},
            1
        ), "light"
    );

    scene->addObject(
        new Flat(
            new MtlExtGGX(
                TextureMap(new Texture("textures/Wood_Floor/Wood_Floor_010_basecolor.jpg"), Spectrum(1)),
                TextureMap(Spectrum(0.1)),
                TextureMap(new Texture("textures/Wood_Floor/Wood_Floor_010_roughness.jpg"), Spectrum(1)),
                1.5, 1,
                TextureMap(new Texture("textures/Wood_Floor/Wood_Floor_010_normal.jpg"), Spectrum(1))
            ), nullptr,
            medium, medium,
            Vec3(-20, -1, -20),
            Vec3(-20, -1, 20),
            Vec3(20, -1, 20),
            Vec3(20, -1, -20)
        ), "down"
    );

//    scene->addObject(
//        new Flat(
//            new GGX(rgb256(250, 250, 250), 0.8), nullptr,
//            medium, medium,
//            Vec3(-20, 10, 20),
//            Vec3(-20, 10, -20),
//            Vec3(20, 10, -20),
//            Vec3(20, 10, 20)
//        ), "up"
//    );
//
//    scene->addObject(
//        new Flat(
//            new GGX(rgb256(170, 170, 250), 0.8), nullptr,
//            medium, medium,
//            Vec3(20, -1, -20),
//            Vec3(20, -1, 20),
//            Vec3(20, 10, 20),
//            Vec3(20, 10, -20)
//        ), "right"
//    );
//
//    scene->addObject(
//        new Flat(
//            new GGX(rgb256(250, 170, 170), 0.8), nullptr,
//            medium, medium,
//            Vec3(-20, -1, 20),
//            Vec3(-20, -1, -20),
//            Vec3(-20, 10, -20),
//            Vec3(-20, 10, 20)
//        ), "left"
//    );
//
//    scene->addObject(
//        new Flat(
//            new Lambert(rgb256(250, 250, 250)), nullptr,
//            medium, medium,
//            Vec3(-20, 10, -20),
//            Vec3(-20, -1, -20),
//            Vec3(20, -1, -20),
//            Vec3(20, 10, -20)
//        ), "back"
//    );
//
//    scene->addObject(
//        new Flat(
//            new Lambert(rgb256(250, 250, 250)), nullptr,
//            medium, medium,
//            Vec3(-20, -1, 20),
//            Vec3(-20, 10, 20),
//            Vec3(20, 10, 20),
//            Vec3(20, -1, 20)
//        ),
//        "front"
//    );

    auto tracer = new BidirectionalPathTracer(2560, 1920, scene);

    char output[100];
    sprintf(output, "results/caustics.png");

    int epoch = 10000;

    cerr << "[T + " << time(nullptr) - T0 << "] | target : " << epoch << endl;
    for (int i = 1; i <= epoch; ++i) {
        tracer->epoch();
        cerr << "[T + " << time(nullptr) - T0 << "] | epoch " << i << endl;
        tracer->savePNG(output, 3);
    }
}
