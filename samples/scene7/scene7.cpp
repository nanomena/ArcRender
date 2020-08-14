#include "arc.hpp"
#include <bits/stdc++.h>
using namespace std;

int main()
{
    ios::sync_with_stdio(0);

    shared_ptr<BxDF> bxdf = make_shared<BSDF>();

    shared_ptr<Material> Mvacant = make_shared<Material>(
        0, Spectrum(0), 1, 0,
        Spectrum(1), Spectrum(1),
        Spectrum(1), "vacant"
    );

    shared_ptr<Material> Mwater = make_shared<Material>(
        0, Spectrum(0), 1.3, 0,
        Spectrum(1), Spectrum(1),
        Spectrum(1), "water"
    );

    shared_ptr<Material> Mlight0 = make_shared<Material>(
        1, Spectrum(0), 1, 0,
        Spectrum(0), Spectrum(0),
        Spectrum(0), "light0"
    );

    shared_ptr<Material> Mlight1 = make_shared<Material>(
        1, Spectrum(1), 1, 0,
        Spectrum(0), Spectrum(0),
        Spectrum(0), "light1"
    );

    shared_ptr<Material> Msilver = make_shared<Material>(
        0, Spectrum(0), 30, 1,
        rgb888(252, 250, 245), rgb888(252, 250, 245),
        Spectrum(1), "silver"
    );

    shared_ptr<Material> Mred = make_shared<Material>(
        0, Spectrum(0), 1.3, 1,
        rgb888(255, 150, 150), rgb888(255),
        Spectrum(1), "red"
    );

    shared_ptr<Material> Mblue = make_shared<Material>(
        0, Spectrum(0), 1.3, 1,
        rgb888(150, 150, 255), rgb888(255),
        Spectrum(1), "blue"
    );

    shared_ptr<Material> Mwhite = make_shared<Material>(
        0, Spectrum(0), 1.3, 1,
        rgb888(255), rgb888(255),
        Spectrum(1), "white"
    );

    shared_ptr<Object> skybox = make_shared<Object>(
        bxdf,
        make_shared<Sphere>(Vec3(0, 0, 0), INF / 10),
        make_shared<Solid>(Mvacant, Mlight0),
        "skybox"
    );

    shared_ptr<Sence> sence = make_shared<Sence>(
        Mvacant, skybox
    );

    sence->add_object(
        make_shared<Object>(
            bxdf,
            make_shared<Flat>(
                Vec3(-0.3, 0.99, -0.7),
                Vec3(-0.3, 0.99, -1.3),
                Vec3(0.3, 0.99, -1.3),
                Vec3(0.3, 0.99, -0.7)
            ),
            make_shared<Solid>(Mlight1, Mlight1, 0),
            "light"
        )
    );

    sence->add_object(
        make_shared<Object>(
            bxdf,
            make_shared<Flat>(
                Vec3(-1, 1, 2),
                Vec3(-1, 1, -2),
                Vec3(1, 1, -2),
                Vec3(1, 1, 2)
            ),
            make_shared<Solid>(Mwhite, Mvacant, 1),
            "up"
        )
    );

    sence->add_object(
        make_shared<Object>(
            bxdf,
            make_shared<Flat>(
                Vec3(-1, 1, -2),
                Vec3(-1, -1, -2),
                Vec3(1, -1, -2),
                Vec3(1, 1, -2)
            ),
            make_shared<Solid>(Msilver, Mvacant, 0.5),
            "back"
        )
    );

    sence->add_object(
        make_shared<Object>(
            bxdf,
            make_shared<Flat>(
                Vec3(1, -1, -2),
                Vec3(1, -1, 2),
                Vec3(1, 1, 2),
                Vec3(1, 1, -2)
            ),
            make_shared<Solid>(Mred, Mvacant, 1),
            "right"
        )
    );

    sence->add_object(
        make_shared<Object>(
            bxdf,
            make_shared<Flat>(
                Vec3(-1, -1, 2),
                Vec3(-1, -1, -2),
                Vec3(-1, 1, -2),
                Vec3(-1, 1, 2)
            ),
            make_shared<Solid>(Mblue, Mvacant, 1),
            "left"
        )
    );

    sence->add_object(
        make_shared<Object>(
            bxdf,
            make_shared<Flat>(
                Vec3(-1, -1, -2),
                Vec3(-1, -1, 2),
                Vec3(1, -1, 2),
                Vec3(1, -1, -2)
            ),
            make_shared<Solid>(Mwhite, Mvacant, 1),
            "down"
        )
    );

    sence->add_object(
        make_shared<Object>(
            bxdf,
            make_shared<Flat>(
                Vec3(-1, -1, 2),
                Vec3(-1, 1, 2),
                Vec3(1, 1, 2),
                Vec3(1, -1, 2)
            ),
            make_shared<Solid>(Mwhite, Mvacant, 1),
            "front"
        )
    );

//    sence->add_object(
//        make_shared<Object>(
//            bxdf,
//            make_shared<Sphere>(
//                Vec3(0, 0, -1), 0.5
//            ),
//            make_shared<Solid>(Mwater, Mvacant, 0.3),
//            "ball"
//        )
//    );

    ObjLoader loader;
    loader.load(
        "teapot.obj", bxdf, Mvacant,
        Trans3(
            Vec3(0.0125, 0, 0),
            Vec3(0, 0.0125, 0),
            Vec3(0, 0, 0.0125),
            Vec3(0, -0.75, -1)
        )
    );
    loader.import_to(sence);

    shared_ptr<Camera> camera = make_shared<PerspectiveCamera>(
        Vec3(0, 0, 1.5),
        Vec3(0.8, 0, 0),
        Vec3(0, 0.6, 0),
        0.6
    );

    shared_ptr<oBuffer> image = make_shared<oBuffer>(800, 600, camera);
    shared_ptr<Render> render = make_shared<Render>(image, sence);

    char output[100];
    sprintf(output, "result.png");

    int epoch = 3000, cluster = 1;
    cerr << "[T + " << (clock() / (double)CLOCKS_PER_SEC) << "] | target : " << epoch << endl;
    for (int i = 1; i <= epoch; ++i)
    {
        render->epoch(cluster);
        cerr << "[T + " << (clock() / (double)CLOCKS_PER_SEC) << "] | epoch " << i << endl;
        if (i % 5 == 0)
            image->save(output, 0.25);
    }
}