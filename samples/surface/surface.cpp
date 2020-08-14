#include "arc.hpp"
#include <bits/stdc++.h>
using namespace std;

int main()
{
    ios::sync_with_stdio(0);

    shared_ptr<BxDF> bxdf = make_shared<BSDF>();

    shared_ptr<Material> Mvacant = make_shared<Material>(
        0, Spectrum(0), 1, 0, Spectrum(1), Spectrum(1), Spectrum(1), "vacant"
    );

    shared_ptr<Material> Mlight0 = make_shared<Material>(
        1, Spectrum(0), 1, 0, Spectrum(0), Spectrum(0), Spectrum(0), "light0"
    );

    shared_ptr<Material> Mlight1 = make_shared<Material>(
        1, Spectrum(1), 1, 0, Spectrum(0), Spectrum(0), Spectrum(0), "light1"
    );

    shared_ptr<Material> Mlight2 = make_shared<Material>(
        1, Spectrum(4), 1, 0, Spectrum(0), Spectrum(0), Spectrum(0), "light2"
    );

    shared_ptr<Material> Mlight3 = make_shared<Material>(
        1, Spectrum(9), 1, 0, Spectrum(0), Spectrum(0), Spectrum(0), "light3"
    );

    shared_ptr<Material> Mdiffuse = make_shared<Material>(
        0, Spectrum(0), 30, 1, rgb888(230), rgb888(230), Spectrum(1), "diffuse"
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

    shared_ptr<Object> back = make_shared<Object>(
        bxdf,
        make_shared<Flat>(
            Vec3(-60, -60, -25),
            Vec3(-60, 60, -25),
            Vec3(60, 60, -25),
            Vec3(60, -60, -25)
        ),
        make_shared<Solid>(Mdiffuse, Mvacant, 1),
        "back"
    );
    // sence->add_object(back);


    double ior, diffuse, rough;
    cout << "ior?" << endl;
    cin >> ior;
    cout << "diffuse?" << endl;
    cin >> diffuse;
    cout << "rough?" << endl;
    cin >> rough;
    char output[100];
    sprintf(
        output, "%03.0lf|%02.0lf|%02.0lf.png", ior * 10, diffuse * 10,
        rough * 10
    );

    shared_ptr<Material> Mtest = make_shared<Material>(
        0, Spectrum(0), ior, diffuse,
        rgb888(252, 250, 245), rgb888(252, 250, 245),
        Spectrum(0.97, 0.995, 0.97), "test"
    );
    shared_ptr<Object> ball = make_shared<Object>(
        bxdf,
        make_shared<Sphere>(Vec3(0, 0, 0), 20),
        make_shared<Solid>(Mtest, Mvacant, rough),
        "ball"
    );
    sence->add_object(ball);

    shared_ptr<Object> light = make_shared<Object>(
        bxdf,
        make_shared<Disc>(Vec3(-35.36, 0, 35.36), Vec3(0.707, 0, -0.707), 20),
        make_shared<Solid>(Mlight2, Mvacant),
        "light"
    );
    sence->add_object(light);

    shared_ptr<Camera> camera = make_shared<PerspectiveCamera>(
        Vec3(0, 0, 50),
        Vec3(1, 0, 0),
        Vec3(0, 1, 0),
        1
    );

    shared_ptr<oBuffer> image = make_shared<oBuffer>(400, 400, camera);
    shared_ptr<Render> render = make_shared<Render>(image, sence);

    int epoch = 1000, cluster = 1;
    cerr << "[T + " << (clock() / (double)CLOCKS_PER_SEC) << "] | target : " << epoch << endl;
    for (int i = 1; i <= epoch; ++i)
    {
        render->epoch(cluster);
        cerr << "[T + " << (clock() / (double)CLOCKS_PER_SEC) << "] | epoch " << i << endl;
        if (i % 5 == 0)
            image->save(output, 1);
    }
}
