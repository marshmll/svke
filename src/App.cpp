#include "App.hpp"

vk::App::App()
{
    createWindow();
    createDevice();
    createRenderer();
    createGlobalPool();
    loadObjects();
}

void vk::App::run()
{
    std::array<std::unique_ptr<Buffer>, Swapchain::MAX_FRAMES_IN_FLIGHT> global_ubo_buffers;

    for (auto &buffer : global_ubo_buffers)
    {
        buffer = std::make_unique<Buffer>(*device, sizeof(GlobalUBO),
                                          VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                          VMA_MEMORY_USAGE_AUTO,
                                          VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT |
                                              VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
        buffer->map();
    }

    auto global_set_layout = DescriptorSetLayout::Builder(*device)
                                 .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
                                 .build();

    std::vector<VkDescriptorSet> global_descriptor_sets(Swapchain::MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < global_descriptor_sets.size(); ++i)
    {
        auto buffer_info = global_ubo_buffers[i]->getDescriptorInfo();
        DescriptorWriter(*global_set_layout, *globalPool).writeBuffer(0, buffer_info).build(global_descriptor_sets[i]);
    }

    Camera camera;
    Object viewer;
    viewer.setTranslation({0.f, 0.f, -2.f});

    Keyboard keyboard(*window);
    Mouse mouse(*window);
    MovementController camera_controller(keyboard, mouse);

    RenderSystem render_system(*device, *renderer, *global_set_layout);
    PointLightSystem point_light_system(*device, *renderer, *global_set_layout);

    Timer delta_timer;

    if (Mouse::isRawMotionSupported())
    {
        mouse.setRawMode(true);
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
        camera.setPerspectiveProjection(Angle::toRadians(50.f), aspect_ratio, .001f, 1000.f);

        const float dt = glm::min(delta_timer.getElapsedTimeAsSeconds(), 0.25f);
        delta_timer.restart();

        mouse.updateCursorData();
        camera_controller.moveInPlaneXZ(dt, viewer);
        camera.setViewYXZ(viewer.getTranslation(), viewer.getRotation());

        if (auto command_buffer = renderer->beginFrame())
        {
            auto current_frame_index = renderer->getCurrentFrameIndex();

            FrameInfo frame_info{
                current_frame_index, dt, command_buffer, camera, global_descriptor_sets[current_frame_index], objects};

            // Update
            GlobalUBO ubo = {};
            ubo.projectionMatrix = camera.getProjectionMatrix();
            ubo.viewMatrix = camera.getViewMatrix();

            point_light_system.update(frame_info, ubo);

            global_ubo_buffers[current_frame_index]->write((void *)&ubo, sizeof(ubo));

            // Render
            renderer->beginRenderPass(command_buffer);

            render_system.render(frame_info);
            point_light_system.render(frame_info);

            renderer->endRenderPass(command_buffer);
            renderer->endFrame();
        }

        if (window->shouldClose())
            std::cout << "Last recorded FPS: " << 1.f / dt << std::endl;
    }
}

void vk::App::createWindow()
{
    window = std::make_unique<Window>(846, 484, "Simple Vulkan Engine");
}

void vk::App::createDevice()
{
    device = std::make_unique<Device>(*window);
}

void vk::App::createRenderer()
{
    renderer = std::make_unique<Renderer>(*device, *window, Swapchain::PresentMode::Immediate);
}

void vk::App::createGlobalPool()
{
    globalPool = DescriptorPool::Builder(*device)
                     .setMaxSets(Swapchain::MAX_FRAMES_IN_FLIGHT)
                     .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Swapchain::MAX_FRAMES_IN_FLIGHT)
                     .build();
}

void vk::App::loadObjects()
{
    {
        std::shared_ptr<Model> quad_model = std::make_shared<Model>(*device);
        if (!quad_model->loadFromFile("assets/models/quad.obj"))
            throw std::runtime_error("vl::App::loadObjects: Failed to load Quad model");

        Object floor;
        floor.setModel(quad_model);
        floor.setTranslation({0.f, 0.5f, 0.f});
        floor.setScale({8.f, 1.f, 8.f});
        objects[floor.getId()] = std::move(floor);
    }
    {
        std::shared_ptr<Model> flat_vase_model = std::make_shared<Model>(*device);
        if (!flat_vase_model->loadFromFile("assets/models/flat_vase.obj"))
            throw std::runtime_error("vl::App::loadObjects: Failed to load Flat Vase model");

        Object flat_vase;
        flat_vase.setModel(flat_vase_model);
        flat_vase.setTranslation({0.5f, 0.5f, 0.f});
        flat_vase.setScale({3.f, 3.f, 3.f});
        objects[flat_vase.getId()] = std::move(flat_vase);
    }
    {
        std::shared_ptr<Model> smooth_vase_model = std::make_shared<Model>(*device);
        if (!smooth_vase_model->loadFromFile("assets/models/smooth_vase.obj"))
            throw std::runtime_error("vl::App::loadObjects: Failed to load Smooth Vase model");

        Object smooth_vase;
        smooth_vase.setModel(smooth_vase_model);
        smooth_vase.setTranslation({-0.5f, 0.5f, 0.f});
        smooth_vase.setScale({3.f, 3.f, 3.f});
        objects[smooth_vase.getId()] = std::move(smooth_vase);
    }
    {
        std::shared_ptr<Model> generator_model = std::make_shared<Model>(*device);
        if (!generator_model->loadFromFile("assets/models/emergency_backup_generator.obj"))
            throw std::runtime_error("vl::App::loadObjects: Failed to load Emergency Backup Generator model");

        Object generator;
        generator.setModel(generator_model);
        generator.setTranslation({0.f, -0.5f, 3.f});
        generator.setScale({0.5f, 0.5f, 0.5f});
        generator.setRotation({0.f, Angle::Rad90, 0.f});
        objects[generator.getId()] = std::move(generator);
    }

    std::vector<Color> light_colors{COLOR_RED,  COLOR_ORANGE, COLOR_YELLOW, COLOR_GREEN,
                                    COLOR_CYAN, COLOR_BLUE,   COLOR_PURPLE};

    for (int i = 0; i < light_colors.size(); i++)
    {
        Object point_light = Object::makePointLight(0.4f);

        point_light.setColor(light_colors[i]);

        auto rotate_light =
            Matrix::rotate(Matrix::identityMat4f(), (i * Angle::Rad360) / light_colors.size(), {0.f, -1.f, 0.f});

        point_light.setTranslation(Vec3f(rotate_light * Vec4f(-1.5f, -1.5f, -1.5f, 1.f)));
        objects[point_light.getId()] = std::move(point_light);
    }
}
