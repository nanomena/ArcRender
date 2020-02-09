
shared_ptr<Material> light_air = make_shared<Material>(
    0.8, Spectrum(1, 1, 1), "light_air"
);
shared_ptr<Material> heavy_air = make_shared<Material>(
    1.0005, Spectrum(1, 1, 1), "heavy_air"
);
shared_ptr<Material> blue_glass = make_shared<Material>(
    1.5, Spectrum(0.95, 0.95, 0.99), "glass"
);
shared_ptr<Material> glass = make_shared<Material>(
    1.5, Spectrum(0.99, 0.99, 0.99), "glass"
);
shared_ptr<Material> emerald = make_shared<Material>(
    1.6, Spectrum(0.97, 0.995, 0.97), "emerald"
);
shared_ptr<Material> water = make_shared<Material>(
    1.3333, Spectrum(0.99, 0.99, 0.99), "water"
);
shared_ptr<Material> diamond = make_shared<Material>(
    2.417, Spectrum(0.99, 0.99, 0.99), "diamond"
);
shared_ptr<Surface> light0 = make_shared<LightSource>(
    Spectrum(0)
);
shared_ptr<Surface> light2 = make_shared<LightSource>(
    Spectrum(4)
);
shared_ptr<Surface> light3 = make_shared<LightSource>(
    Spectrum(9)
);
shared_ptr<Surface> light4 = make_shared<LightSource>(
    Spectrum(16)
);
shared_ptr<Surface> good_mirror = make_shared<MetalSurface>(
    rgb888(250, 250, 250), 0, 0.9
);
shared_ptr<Surface> iron_surface11 = make_shared<MetalSurface>(
    rgb888(198, 198, 200), 0.1, 0.1
);
shared_ptr<Surface> iron_surface99 = make_shared<MetalSurface>(
    rgb888(198, 198, 200), 0.9, 0.9
);
shared_ptr<Surface> iron_surface59 = make_shared<MetalSurface>(
    rgb888(198, 198, 200), 0.5, 0.9
);
shared_ptr<Surface> iron_surface19 = make_shared<MetalSurface>(
    rgb888(198, 198, 200), 0.1, 0.9
);
shared_ptr<Surface> gold_surface19 = make_shared<MetalSurface>(
    rgb888(255, 229, 158), 0.1, 0.9
);
shared_ptr<Surface> gold_surface59 = make_shared<MetalSurface>(
    rgb888(255, 229, 158), 0.5, 0.9
);
shared_ptr<Surface> gold_surface99 = make_shared<MetalSurface>(
    rgb888(255, 229, 158), 0.9, 0.9
);
shared_ptr<Surface> diffuse_surface = make_shared<MetalSurface>(
    rgb888(230), 1, 0
);
shared_ptr<Surface> red_diffuse_surface = make_shared<MetalSurface>(
    rgb888(230, 133, 133), 1, 0
);
shared_ptr<Surface> blue_diffuse_surface = make_shared<MetalSurface>(
    rgb888(133, 133, 230), 1, 0
);
shared_ptr<Shape> skybox = make_shared<Sphere>(
    Vec3(0, 0, 0), INF / 10
);
shared_ptr<Sence> sence = make_shared<Sence>(
    make_shared<Object>(skybox, light0, "skybox")
);


shared_ptr<Sence> load_sence1()
{
    shared_ptr<Shape> light = make_shared<Flat>(4,
        Vec3(-6, 14.9, 0),
        Vec3(-6, 14.9, 14),
        Vec3(6, 14.9, 14),
        Vec3(6, 14.9, 0)
    );
    sence->add_object(make_shared<Object>(light, light4, "light"));

    shared_ptr<Shape> ball1 = make_shared<Sphere>(
        Vec3(7, -8, 13), 7
    );
    sence->add_object(make_shared<Object>(ball1, gold_surface59, "ball1"));

    shared_ptr<Shape> ball2 = make_shared<Sphere>(
        Vec3(-6, -8, 3), 7
    );
    sence->add_object(make_shared<Object>(ball2, make_shared<TransSurface>(emerald, 0.1), "ball2"));

    shared_ptr<Shape> front = make_shared<Flat>(4,
        Vec3(-15, -15, 20),
        Vec3(-15, 15, 20),
        Vec3(15, 15, 20),
        Vec3(15, -15, 20)
    );
    sence->add_object(make_shared<Object>(front, iron_surface59, "front"));

    shared_ptr<Shape> left = make_shared<Flat>(4,
        Vec3(-15, 15, -10),
        Vec3(-15, 15, 20),
        Vec3(-15, -15, 20),
        Vec3(-15, -15, -10)
    );
    sence->add_object(make_shared<Object>(left, blue_diffuse_surface, "left"));

    shared_ptr<Shape> right = make_shared<Flat>(4,
        Vec3(15, 15, 20),
        Vec3(15, 15, -10),
        Vec3(15, -15, -10),
        Vec3(15, -15, 20)
    );
    sence->add_object(make_shared<Object>(right, red_diffuse_surface, "right"));

    shared_ptr<Shape> up = make_shared<Flat>(4,
        Vec3(-15, 15, 20),
        Vec3(-15, 15, -10),
        Vec3(15, 15, -10),
        Vec3(15, 15, 20)
    );
    sence->add_object(make_shared<Object>(up, diffuse_surface, "up"));

    shared_ptr<Shape> down = make_shared<Flat>(4,
        Vec3(-15, -15, -10),
        Vec3(-15, -15, 20),
        Vec3(15, -15, 20),
        Vec3(15, -15, -10)
    );
    sence->add_object(make_shared<Object>(down, diffuse_surface, "down"));

    return sence;
}

shared_ptr<Sence> load_surface_test()
{
    // shared_ptr<Shape> light = make_shared<Disc>(
    //     Vec3(0, 0, -51), Vec3(0, 0, 1), 20
    // );
    shared_ptr<Shape> light = make_shared<Disc>(
        Vec3(-35.36, 0, -35.36), Vec3(0.707, 0, 0.707), 20
    );
    sence->add_object(make_shared<Object>(light, light3, "light"));

    // shared_ptr<Surface> test_surface = make_shared<MetalSurface>(
    //     rgb888(255, 229, 158), 1, 1
    // );

    shared_ptr<Shape> back = make_shared<Flat>(4,
        Vec3(-40, -40, 25),
        Vec3(-40, 40, 25),
        Vec3(40, 40, 25),
        Vec3(40, -40, 25)
    );
    sence->add_object(make_shared<Object>(back, diffuse_surface, "back"));

    shared_ptr<Surface> test_surface = make_shared<TransSurface>(
        emerald, 0.3
    );

    shared_ptr<Shape> ball1 = make_shared<Sphere>(
        Vec3(0, 0, 0), 20
    );
    sence->add_object(make_shared<Object>(ball1, test_surface, "ball1"));

    return sence;
}

#include "arc/objsence.hpp"
shared_ptr<Sence> load_teapot()
{
    double d[3][3] = {
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}
    };
    Mat3 T(d);
    ObjSence x;
    x.import("objects/teapot.obj", sence, T);

    cerr << "finish importing!" << endl;

    shared_ptr<Shape> light = make_shared<Flat>(4,
        Vec3(-100, 200, -100),
        Vec3(-100, 200, 100),
        Vec3(100, 200, 100),
        Vec3(100, 200, -100)
    );
    sence->add_object(make_shared<Object>(light, light2, "light"));

    return sence;
}
