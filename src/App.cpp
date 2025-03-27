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
    Keyboard kb(*window);
    KeyboardMovementController camera_controller(kb);
    RenderSystem render_system(*device, *renderer);
    Clock delta_clk;

    while (!window->shouldClose())
    {
        window->pollEvents();

        const float aspect_ratio = renderer->getAspectRatio();
        camera.setPerspectiveProjection(glm::radians(45.f), aspect_ratio, .1f, 10.f);

        const float dt = delta_clk.getElapsedTimeAsSeconds();
        delta_clk.restart();

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
    std::shared_ptr<Model> model = Model::createCubeModel(*device, {0.f, 0.f, 0.f});

    Object cube;
    cube.setModel(model);
    cube.setTranslation({0.f, 0.f, 2.5f});
    cube.setScale({0.5f, 0.5f, 0.5f});
    objects.push_back(std::move(cube));
}
