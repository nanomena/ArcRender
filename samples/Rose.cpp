#include "arc.hpp"
#include <bits/stdc++.h>
using namespace std;

int main()
{
    ios::sync_with_stdio(0);

    shared_ptr<BxDF> bxdf = make_shared<BSDF>();

    shared_ptr<Material> Mair = make_shared<Material>(
        0, rgb(0), 1, 0, rgb(1), rgb(1), Spectrum(1), "air"
    );

    shared_ptr<Material> Msky = make_shared<Material>(
        1, rgb(0), 1, 0, rgb(0), rgb(0), Spectrum(0), "sky"
    );


    shared_ptr<Material> Mlight = make_shared<Material>(
        1, rgb(1) * 4, 1, 0, rgb(0), rgb(0), Spectrum(0), "light"
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

    loader.load("Rose_Model_3_decimated.obj", bxdf);
    loader.import(sence);

    shared_ptr<Object> light = make_shared<Object>(
        bxdf,
        make_shared<Flat>(4,
            Vec3(-200, 600,  200),
            Vec3(-200, 600, -200),
            Vec3(200, 600, -200),
            Vec3(200, 600, 200)
        ),
        make_shared<Thin>(Mlight),
        "light"
    );
    sence->add_object(light);

    shared_ptr<Camera> camera = make_shared<PerspectiveCamera>(
        Vec3(-0.75, -4, 15),
        Vec3(1, 0, 0),
        Vec3(0, 0.7071067811865476, -0.7071067811865476),
        1
    );

    shared_ptr<oBuffer> image = make_shared<oBuffer>(1000, 1000, camera);
    shared_ptr<Render> render = make_shared<Render>(image, sence);

    char output[100];
    int epoch = 10000, cluster = 1;
    cerr << "target : " << epoch << endl;
    for (int i = 1; i <= epoch; ++ i)
    {
        render->epoch(cluster);
        cerr << "epoch " << i << endl;
        sprintf(output, "rose.ppm");
        image->save(output, 0.24);
        if (i % 40 == 0)
        {
            sprintf(output, "rose%d.ppm", i);
            image->save(output, 0.24);
        }
    }
    image->save(output, 0.24);
}
