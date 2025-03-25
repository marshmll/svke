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
    renderer = std::make_unique<Renderer>(*device, *window, COLOR_WHITE);
}

void fl::App::loadDrawables()
{
    VertexArray vertices =
        VertexArray{{Vertex{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}}, Vertex{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                     Vertex{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}}};

    auto model = std::make_shared<Model>(*device);
    model->loadFromData(vertices);

    Drawable triangle;
    triangle.setModel(model);
    triangle.setColor(COLOR_RED);
    triangle.translate({0.f, 0.f});
    triangle.setScale({1.f, 0.5f});
    triangle.rotate(70.f);

    drawables.push_back(std::move(triangle));
}
