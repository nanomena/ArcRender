#include "arc.hpp"
#include <bits/stdc++.h>
using namespace std;

int main()
{
    ios::sync_with_stdio(0);

    shared_ptr<BxDF> bxdf = make_shared<BSDF>();

    shared_ptr<Material> Mair = make_shared<Material>(
        0, Spectrum(0), 1, 0, Spectrum(1), Spectrum(1), Spectrum(1), "air"
    );

    shared_ptr<Material> Msky = make_shared<Material>(
        1, Spectrum(0.007, 0.008, 0.01), 1, 0, Spectrum(0), Spectrum(0), Spectrum(0), "sky"
    );

    shared_ptr<Material> Mlight = make_shared<Material>(
        1, Spectrum(0.03), 1, 0, Spectrum(0), Spectrum(0), Spectrum(0), "light"
    );

    shared_ptr<Object> skybox = make_shared<Object>(
        bxdf,
        make_shared<Sphere>(Vec3(0, 0, 0), INF / 10),
        make_shared<Solid>(Mair, Msky),
        "skybox"
    );

    shared_ptr<Sence> sence = make_shared<Sence>(
        Mair, skybox
    );

    ObjLoader loader;

    loader.load("HelloWorld.obj", bxdf);
    loader.import(sence);

    shared_ptr<Object> light = make_shared<Object>(
        bxdf,
        make_shared<Flat>(4,
            Vec3(-100, 200, 100),
            Vec3(-100, 200, -100),
            Vec3(100, 200, -100),
            Vec3(100, 200, 100)
        ),
        make_shared<Thin>(Mlight),
        "light"
    );
    sence->add_object(light);

    shared_ptr<Camera> camera = make_shared<PerspectiveCamera>(
        Vec3(0, 5, -20),
        Vec3(1, 0, 0),
        Vec3(0, 1, 0),
        1
    );

    shared_ptr<oBuffer> image = make_shared<oBuffer>(800, 800, camera);
    shared_ptr<Render> render = make_shared<Render>(image, sence);

    char output[100]; sprintf(output, "HelloWorld.ppm");
    int epoch = 10000, cluster = 1;
    cerr << "target : " << epoch << endl;
    for (int i = 1; i <= epoch; ++ i)
    {
        render->epoch(cluster);
        cerr << "epoch " << i << endl;
        if (i % 3 == 0)
            image->save(output, 0.12);
    }
    image->save(output, 0.12);
}
