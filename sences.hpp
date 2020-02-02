
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
shared_ptr<Surface> iron_surface11 = make_shared<BRDFSurface>(
    rgb888(198, 198, 200), 0.1, 0.1
);
shared_ptr<Surface> iron_surface99 = make_shared<BRDFSurface>(
    rgb888(198, 198, 200), 0.9, 0.9
);
shared_ptr<Surface> iron_surface59 = make_shared<BRDFSurface>(
    rgb888(198, 198, 200), 0.5, 0.9
);
shared_ptr<Surface> iron_surface19 = make_shared<BRDFSurface>(
    rgb888(198, 198, 200), 0.1, 0.9
);
shared_ptr<Surface> diffuse_surface = make_shared<BRDFSurface>(
    rgb888(230), 1, 0
);
shared_ptr<Surface> red_diffuse_surface = make_shared<BRDFSurface>(
    rgb888(230, 133, 133), 1, 0
);
shared_ptr<Surface> blue_diffuse_surface = make_shared<BRDFSurface>(
    rgb888(133, 133, 230), 1, 0
);
shared_ptr<Shape> skybox = make_shared<Sphere>(
    Vec3(0, 0, 0), INF
);
shared_ptr<Sence> sence = make_shared<Sence>(
    make_shared<Object>(skybox, light0, "skybox")
);


shared_ptr<Sence> load_sence1()
{
    shared_ptr<Shape> light = make_shared<Flat>(4,
        Vec3(-6, -14.9, 0),
        Vec3(-6, -14.9, 14),
        Vec3(6, -14.9, 14),
        Vec3(6, -14.9, 0)
    );
    sence->add_object(make_shared<Object>(light, light4, "light"));

    shared_ptr<Shape> ball1 = make_shared<Sphere>(
        Vec3(7, 9, 13), 6
    );
    sence->add_object(make_shared<Object>(ball1, iron_surface99, "ball1"));

    shared_ptr<Shape> ball2 = make_shared<Sphere>(
        Vec3(-5, 9, 5), 6
    );
    sence->add_object(make_shared<Object>(ball2, diffuse_surface, "ball2"));

    shared_ptr<Shape> front = make_shared<Flat>(4,
        Vec3(-15, -15, 20),
        Vec3(-15, 15, 20),
        Vec3(15, 15, 20),
        Vec3(15, -15, 20)
    );
    sence->add_object(make_shared<Object>(front, diffuse_surface, "front"));

    shared_ptr<Shape> left = make_shared<Flat>(4,
        Vec3(-15, -15, 20),
        Vec3(-15, -15, -40),
        Vec3(-15, 15, -40),
        Vec3(-15, 15, 20)
    );
    sence->add_object(make_shared<Object>(left, blue_diffuse_surface, "left"));

    shared_ptr<Shape> right = make_shared<Flat>(4,
        Vec3(15, -15, -40),
        Vec3(15, -15, 20),
        Vec3(15, 15, 20),
        Vec3(15, 15, -40)
    );
    sence->add_object(make_shared<Object>(right, red_diffuse_surface, "right"));

    shared_ptr<Shape> back = make_shared<Flat>(4,
        Vec3(-15, 15, -40),
        Vec3(-15, -15, -40),
        Vec3(15, -15, -40),
        Vec3(15, 15, -40)
    );
    sence->add_object(make_shared<Object>(back, diffuse_surface, "back"));

    shared_ptr<Shape> down = make_shared<Flat>(4,
        Vec3(-15, -15, -40),
        Vec3(-15, -15, 20),
        Vec3(15, -15, 20),
        Vec3(15, -15, -40)
    );
    sence->add_object(make_shared<Object>(down, diffuse_surface, "down"));

    shared_ptr<Shape> up = make_shared<Flat>(4,
        Vec3(-15, 15, 20),
        Vec3(-15, 15, -40),
        Vec3(15, 15, -40),
        Vec3(15, 15, 20)
    );
    sence->add_object(make_shared<Object>(up, diffuse_surface, "up"));

    return sence;
}

shared_ptr<Sence> load_surface_test()
{
    shared_ptr<Shape> light = make_shared<Disc>(
        Vec3(0, 0, -50), Vec3(0, 0, 1), 20
    );
    sence->add_object(make_shared<Object>(light, light3, "light"));

    shared_ptr<Surface> test_surface = make_shared<BRDFSurface>(
        rgb888(255, 229, 158), 0, 0
    );

    shared_ptr<Shape> ball1 = make_shared<Sphere>(
        Vec3(0, 0, 15), 20
    );
    sence->add_object(make_shared<Object>(ball1, test_surface, "ball1"));

    return sence;
}
