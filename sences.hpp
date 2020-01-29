

shared_ptr<Sence> load_sence1()
{

    shared_ptr<Surface> dim_light = make_shared<LightSource>(
        Light(0)
    );
    shared_ptr<Surface> white_light = make_shared<LightSource>(
        Light(2)
    );
    shared_ptr<Surface> diffuse_surface = make_shared<NaiveSurface>(
        Light(1), 0.8, 0
    );
    shared_ptr<Surface> red_diffuse_surface = make_shared<NaiveSurface>(
        Light(1, 0.58, 0.58), 0.8, 0
    );
    shared_ptr<Surface> blue_diffuse_surface = make_shared<NaiveSurface>(
        Light(0.58, 0.58, 1), 0.8, 0
    );
    shared_ptr<Surface> specular_surface = make_shared<NaiveSurface>(
        Light(1), 0, 0.8
    );
    shared_ptr<Surface> specular_surface_7 = make_shared<NaiveSurface>(
        Light(1), 0.1, 0.7
    );
    shared_ptr<Surface> specular_surface_1 = make_shared<NaiveSurface>(
        Light(1), 0.7, 0.1
    );


    shared_ptr<Shape> skybox = make_shared<Sphere>(
        Vec3(0, 0, 0), INF
    );

    shared_ptr<Sence> sence = make_shared<Sence>(
        make_shared<Object>(skybox, dim_light, "skybox")
    );



    shared_ptr<Shape> ball1 = make_shared<Sphere>(
        Vec3(7, 9, 13), 6
    );
    sence->add_object(make_shared<Object>(ball1, specular_surface, "ball2"));


    shared_ptr<Shape> ball2 = make_shared<Sphere>(
        Vec3(-5, 9, 5), 6
    );
    sence->add_object(make_shared<Object>(ball2, specular_surface_1, "ball2"));

    shared_ptr<Shape> front = make_shared<Flat>(4,
        Vec3(-15, -15, 20),
        Vec3(-15, 15, 20),
        Vec3(15, 15, 20),
        Vec3(15, -15, 20)
    );
    sence->add_object(make_shared<Object>(front, specular_surface_7, "front"));

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
    sence->add_object(make_shared<Object>(back, white_light, "back"));

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
