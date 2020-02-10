#include "arc/render.hpp"
#include <vector>
using namespace std;

int main()
{
    int epoch = 1000, cluster = 1;
    cerr << "target : " << epoch << endl;
    ios::sync_with_stdio(0);

    shared_ptr<BxDF> bxdf = make_shared<BSDF>();

    shared_ptr<Material> Mair = make_shared<Material>(
        0, Spectrum(0), 1, 1, 0, Spectrum(1), Spectrum(1), Spectrum(1), "air"
    );

    shared_ptr<Material> Mlight0 = make_shared<Material>(
        1, Spectrum(0), 1, 1, 0, Spectrum(0), Spectrum(0), Spectrum(0), "light0"
    );

    shared_ptr<Material> Mlight1 = make_shared<Material>(
        1, Spectrum(1), 1, 1, 0, Spectrum(0), Spectrum(0), Spectrum(0), "light1"
    );

    shared_ptr<Material> Mlight2 = make_shared<Material>(
        1, Spectrum(4), 1, 1, 0, Spectrum(0), Spectrum(0), Spectrum(0), "light2"
    );

    shared_ptr<Material> Mlight3 = make_shared<Material>(
        1, Spectrum(9), 1, 1, 0, Spectrum(0), Spectrum(0), Spectrum(0), "light3"
    );

    shared_ptr<Material> Mdiffuse = make_shared<Material>(
        0, Spectrum(0), 1, 0, 1, rgb888(230), rgb888(230), Spectrum(1), "diffuse"
    );

    shared_ptr<Material> Mtest = make_shared<Material>(
        0, Spectrum(0), 30, 1, 1, Spectrum(1), Spectrum(1), Spectrum(1), "test"
    );

    shared_ptr<Object> skybox = make_shared<Object>(
        bxdf,
        make_shared<Sphere>(Vec3(0, 0, 0), INF / 10),
        make_shared<Uniform>(Mair, Mlight0),
        "skybox"
    );

    shared_ptr<Sence> sence = make_shared<Sence>(
        Mair, skybox
    );

    shared_ptr<Object> back = make_shared<Object>(
        bxdf,
        make_shared<Flat>(4,
            Vec3(-60, -60, 30),
            Vec3(-60, 60, 30),
            Vec3(60, 60, 30),
            Vec3(60, -60, 30)
        ),
        make_shared<Uniform>(Mdiffuse, Mair, 0),
        "back"
    );
    // sence->add_object(back);

    shared_ptr<Object> ball = make_shared<Object>(
        bxdf,
        make_shared<Sphere>(Vec3(0, 0, 0), 20),
        make_shared<Uniform>(Mtest, Mair, 1.3),
        "ball"
    );
    sence->add_object(ball);

    shared_ptr<Object> light = make_shared<Object>(
        bxdf,
        make_shared<Disc>(Vec3(-35.36, 0, -35.36), Vec3(0.707, 0, 0.707), 20),
        make_shared<Uniform>(Mlight2, Mair),
        "light"
    );
    sence->add_object(light);




    shared_ptr<Camera> camera = make_shared<PerspectiveCamera>(
        Vec3(0, 0, -50),
        Vec3(1, 0, 0),
        Vec3(0, 1, 0),
        1
    );

    shared_ptr<oBuffer> image = make_shared<oBuffer>(400, 400, camera);
    shared_ptr<Render> render = make_shared<Render>(image, sence);
    for (int i = 1; i <= epoch; ++ i)
    {
        render->epoch(cluster);
        cerr << "epoch " << i << endl;
        if (i % 5 == 0)
            image->save("test.ppm", 1);
    }
    image->save("test.ppm", 1);
}
