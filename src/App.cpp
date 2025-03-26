#include "App.hpp"

fl::App::App()
{
    createWindow();
    createDevice();
    createRenderer();
    loadDrawables();
}

fl::App::~App()
{
}

void fl::App::run()
{
    RenderSystem render_system(*device, *renderer);

    while (!window->shouldClose())
    {
        window->pollEvents();

        if (auto command_buffer = renderer->beginFrame())
        {
            renderer->beginRenderPass(command_buffer);

            render_system.render(command_buffer, drawables);

            renderer->endRenderPass(command_buffer);
            renderer->endFrame();
        }
    }
}

void fl::App::createWindow()
{
    window = std::make_unique<Window>(600, 600, "Flare");
}

void fl::App::createDevice()
{
    device = std::make_unique<Device>(*window);
}

void fl::App::createRenderer()
{
    renderer = std::make_unique<Renderer>(*device, *window);
}

void fl::App::loadDrawables()
{
    std::shared_ptr<Model> model = Model::createCubeModel(*device, {0.f, 0.f, 0.f});

    Drawable cube;
    cube.setModel(model);
    cube.setTranslation({0.f, 0.f, 0.5f});
    cube.setScale({0.5f, 0.5f, 0.5f});
    drawables.push_back(std::move(cube));
}
