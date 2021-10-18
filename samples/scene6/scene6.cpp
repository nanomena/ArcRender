#include "arc.hpp"
#include <bits/stdc++.h>
using namespace std;

int main()
{
    ios::sync_with_stdio(0);

    shared_ptr<Object> skybox = make_shared<Object>(
        make_shared<Sphere>(Vec3(0, 0, 0), INF / 10),
        make_shared<Surface>(),
        "skybox"
    );

    shared_ptr<Scene> scene = make_shared<Scene>(skybox);

    scene->add_object(
        make_shared<Object>(
            make_shared<Flat>(
                Vec3(-3, 9.9, -7),
                Vec3(-3, 9.9, -13),
                Vec3(3, 9.9, -13),
                Vec3(3, 9.9, -7)
            ),
            make_light(make_shared<UniformLight>(), Spect(1)),
            "light"
        )
    );

    scene->add_object(
        make_shared<Object>(
            make_shared<Flat>(
                Vec3(-10, -10, -20),
                Vec3(-10, -10, 20),
                Vec3(10, -10, 20),
                Vec3(10, -10, -20)
            ),
            make_bxdf(make_shared<Lambert>(), rgb888(255, 255, 255)),
            "down"
        )
    );

    scene->add_object(
        make_shared<Object>(
            make_shared<Flat>(
                Vec3(-10, 10, 20),
                Vec3(-10, 10, -20),
                Vec3(10, 10, -20),
                Vec3(10, 10, 20)
            ),
            make_bxdf(make_shared<Lambert>(), rgb888(255, 255, 255)),
            "up"
        )
    );

    scene->add_object(
        make_shared<Object>(
            make_shared<Flat>(
                Vec3(10, -10, -20),
                Vec3(10, -10, 20),
                Vec3(10, 10, 20),
                Vec3(10, 10, -20)
            ),
            make_bxdf(make_shared<Lambert>(), rgb888(255, 175, 175)),
            "right"
        )
    );

    scene->add_object(
        make_shared<Object>(
            make_shared<Flat>(
                Vec3(-10, -10, 20),
                Vec3(-10, -10, -20),
                Vec3(-10, 10, -20),
                Vec3(-10, 10, 20)
            ),
            make_bxdf(make_shared<Lambert>(), rgb888(175, 175, 255)),
            "left"
        )
    );

    scene->add_object(
        make_shared<Object>(
            make_shared<Flat>(
                Vec3(-10, 10, -20),
                Vec3(-10, -10, -20),
                Vec3(10, -10, -20),
                Vec3(10, 10, -20)
            ),
            make_bxdf(make_shared<Lambert>(), rgb888(255, 255, 255)),
            "back"
        )
    );

    scene->add_object(
        make_shared<Object>(
            make_shared<Flat>(
                Vec3(-10, -10, 20),
                Vec3(-10, 10, 20),
                Vec3(10, 10, 20),
                Vec3(10, -10, 20)
            ),
            make_bxdf(make_shared<Lambert>(), rgb888(255, 255, 255)),
            "front"
        )
    );

    shared_ptr<Camera> camera = make_shared<PerspectiveCamera>(
        Vec3(0, 0, 15),
        Vec3(0.8, 0, 0),
        Vec3(0, 0.6, 0),
        0.6
    );

    shared_ptr<Image> image = make_shared<Image>(800, 600, camera);
    shared_ptr<Render> render = make_shared<BidirectionalPathTracer>(image, scene);

    char output[100];
    sprintf(output, "result.png");

    int epoch = 3000, cluster = 1;
    cerr << "[T + " << (clock() / (double)CLOCKS_PER_SEC) << "] | target : " << epoch << endl;
    for (int i = 1; i <= epoch; ++i)
    {
        render->epoch(cluster);
        cerr << "[T + " << (clock() / (double)CLOCKS_PER_SEC) << "] | epoch " << i << endl;
        if (i % 1 == 0)
            image->save(output, 0.35);
    }

//    int epoch = 30000, cluster = 1;
//    cerr << "[T + " << (clock() / (double)CLOCKS_PER_SEC) << "] | target : " << epoch << endl;
//    int idx = 800 * 100 + 400;
//    for (int i = 1; i <= epoch; ++i)
//    {
//        render->step_one(idx, cluster);
//        cerr << "[T + " << (clock() / (double)CLOCKS_PER_SEC) << "] | epoch " << i << endl;
//        cerr << image->get(idx) << endl;
//    }
////    (0.0044293,0.0044293,0.0044293)
}