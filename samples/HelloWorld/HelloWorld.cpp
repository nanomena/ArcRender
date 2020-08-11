#include "arc.hpp"
#include <bits/stdc++.h>
using namespace std;

int main()
{
    ios::sync_with_stdio(0);

    shared_ptr<BxDF> bxdf = make_shared<BSDF>();

    shared_ptr<Material> Mvacant = make_shared<Material>(
        0, rgb(0), 1, 0, rgb(1), rgb(1), Spectrum(1), "vacant"
    );

    // shared_ptr<Material> Msky = make_shared<Material>(
    //     1, rgb(0.14, 0.15, 0.16) / 4, 1, 0, rgb(0), rgb(0), Spectrum(0), "sky"
    // );
    shared_ptr<Material> Msky = make_shared<Material>(
        1, rgb(0.14, 0.15, 0.16) * 4, 1, 0, rgb(0), rgb(0), Spectrum(0), "sky"
    );

    shared_ptr<Material> Mlight = make_shared<Material>(
        1, rgb(1) * 10, 1, 0, rgb(0), rgb(0), Spectrum(0), "light"
    );

    shared_ptr<Object> skybox = make_shared<Object>(
        bxdf,
        make_shared<Sphere>(Vec3(0, 0, 0), INF / 10),
        make_shared<Solid>(Mvacant, Msky),
        "skybox"
    );

    shared_ptr<Sence> sence = make_shared<Sence>(
        Mvacant, skybox
    );

    ObjLoader loader;

    loader.load("HelloWorld.obj", bxdf);
    loader.import(sence);

    shared_ptr<Object> light = make_shared<Object>(
        bxdf,
        make_shared<Flat>(
            Vec3(-2000, 30000, -2000),
            Vec3(-2000, 30000, 2000),
            Vec3(2000, 30000, 2000),
            Vec3(2000, 30000, -2000)
        ),
        make_shared<Thin>(Mlight),
        "light"
    );
    sence->add_object(light);

    // shared_ptr<Camera> camera = make_shared<PerspectiveCamera>(
    //     Vec3(60, 60, 100),
    //     Vec3(1.3333333333333333, 0, 0),
    //     Vec3(0, 0.8660254037844386, 0.5),
    //     1
    // );

    shared_ptr<Camera> camera = make_shared<PerspectiveCamera>(
        Vec3(105, 55, -20),
        Vec3(0, 0, -1.3333333333333333),
        Vec3(-0.5, 0.8660254037844386, 0),
        1
    );

    shared_ptr<oBuffer> image = make_shared<oBuffer>(800, 600, camera);
    shared_ptr<Render> render = make_shared<Render>(image, sence);

    char output[100];
    int epoch = 5000, cluster = 1;
    cerr << "[T + " << (clock() / (double)CLOCKS_PER_SEC) << "] | target : " << epoch << endl;
    for (int i = 1; i <= epoch; ++i)
    {
        render->epoch(cluster);
        cerr << "[T + " << (clock() / (double)CLOCKS_PER_SEC) << "] | epoch " << i << endl;
        sprintf(output, "HelloWorld.png");
        image->save(output, 0.24);
        if (i % 1 == 0)
        {
            sprintf(output, "HelloWorld%d.png", i);
            image->save(output, 0.24);
        }
    }
    image->save(output, 0.24);
}
