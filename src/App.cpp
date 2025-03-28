#include "App.hpp"

vk::App::App()
{
    createWindow();
    createDevice();
    createRenderer();
    loadObjects();
}

vk::App::~App()
{
}

void vk::App::run()
{
    Camera camera;
    Object viewer;
    Keyboard keyboard(*window);
    Mouse mouse(*window);
    MovementController camera_controller(keyboard, mouse);
    RenderSystem render_system(*device, *renderer);
    Timer delta_timer;

    if (Mouse::isRawMotionSupported())
    {
        mouse.setCursorMode(Mouse::CursorMode::Disabled);
    }
    else
    {
        std::cerr << "Mouse raw mode is not supported" << std::endl;
    }

    while (!window->shouldClose())
    {
        window->pollEvents();

        if (keyboard.isKeyPressed(Keyboard::Key::Escape))
            mouse.setCursorMode(Mouse::CursorMode::Normal);

        else if (keyboard.isKeyPressed(Keyboard::Key::Enter))
            mouse.setCursorMode(Mouse::CursorMode::Disabled);

        const float aspect_ratio = renderer->getAspectRatio();
        camera.setPerspectiveProjection(glm::radians(45.f), aspect_ratio, .001f, 10.f);

        const float dt = glm::min(delta_timer.getElapsedTimeAsSeconds(), 0.25f);
        delta_timer.restart();

        mouse.updateCursorData();
        camera_controller.moveInPlaneXZ(dt, viewer);
        camera.setViewYXZ(viewer.getTranslation(), viewer.getRotation());

        if (auto command_buffer = renderer->beginFrame())
        {
            renderer->beginRenderPass(command_buffer);

            render_system.render(command_buffer, objects, camera);

            renderer->endRenderPass(command_buffer);
            renderer->endFrame();
        }
    }
}

void vk::App::createWindow()
{
    window = std::make_unique<Window>(846, 484, "Flare");
}

void vk::App::createDevice()
{
    device = std::make_unique<Device>(*window);
}

void vk::App::createRenderer()
{
    renderer = std::make_unique<Renderer>(*device, *window);
}

void vk::App::loadObjects()
{
    std::shared_ptr<Model> model = std::make_shared<Model>(*device);
    model->loadFromFile("assets/models/smooth_vase.obj");

    Object cube;
    cube.setModel(model);
    cube.setTranslation({0.f, 0.f, 2.5f});
    cube.setScale(glm::vec3{3.f});
    objects.push_back(std::move(cube));
}
