#include "arc/render.hpp"
#include "sences.hpp"
#include <vector>
using namespace std;

int main()
{
    ios::sync_with_stdio(0);

    shared_ptr<Sence> sence = load_surface_test();

    shared_ptr<Camera> camera = make_shared<PerspectiveCamera>(
        Vec3(35.35, 0, -20.35),
        Vec3(0.707, 0, 0.707),
        Vec3(0, 1, 0),
        1
    );
    // shared_ptr<Camera> camera = make_shared<PerspectiveCamera>(
    //     Vec3(0, 0, -35),
    //     Vec3(1, 0, 0),
    //     Vec3(0, 1, 0),
    //     1
    // );
    shared_ptr<Image> image = make_shared<Image>(400, 400, camera);

    int epoch = 1000;
    shared_ptr<Render> render = make_shared<Render>(image, sence);
    for (int i = 1; i <= epoch; ++ i)
    {
        render->epoch();
        cerr << "epoch " << i << endl;
        if (i % 5 == 0)
            image->save("test.ppm", 1);
    }
}
