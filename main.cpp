#pragma GCC optimize("Ofast")
#pragma GCC target("sse3","sse2","sse")
#pragma GCC target("avx","sse4","sse4.1","sse4.2","ssse3")
#pragma GCC target("f16c")
#pragma GCC optimize("inline","fast-math","unroll-loops","no-stack-protector")

#include "arc/render.hpp"
#include "sences.hpp"
#include <vector>
using namespace std;

int main()
{
    int epoch = 20000, cluster = 1;
    cerr << "target : " << epoch << endl;
    ios::sync_with_stdio(0);

    // shared_ptr<Sence> sence = load_surface_test();
    // shared_ptr<Camera> camera = make_shared<PerspectiveCamera>(
    //     Vec3(0, 0, -50),
    //     Vec3(1, 0, 0),
    //     Vec3(0, 1, 0),
    //     1
    // );
    // shared_ptr<Image> image = make_shared<Image>(200, 200, camera);

    shared_ptr<Sence> sence = load_sence1();
    shared_ptr<Camera> camera = make_shared<PerspectiveCamera>(
        Vec3(0, 0, -35),
        Vec3(1, 0, 0),
        Vec3(0, 1, 0),
        1
    );
    shared_ptr<Image> image = make_shared<Image>(800, 800, camera);

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
