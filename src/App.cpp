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
    Texture texture;

    if (!texture.loadFromFile("assets/textures/skull.jpg"))
        std::cerr << "Failed to load texture";

    std::cout << "LOADED TEXTURE assets/textures/texture.jpg WITH " << texture.getSize() << " bytes" << std::endl;

    TextureImage image(*device, texture, TextureImage::Format::RGBA, TextureImage::Tiling::Optimal,
                       VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

    TextureSampler::Config sampler_config{};
    TextureSampler::defaultTextureSamplerConfig(sampler_config);
    sampler_config.anisotropyEnable = VK_TRUE;
    sampler_config.maxAnisotropy = device->getProperties().limits.maxSamplerAnisotropy;

    TextureSampler sampler(*device, sampler_config);

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
                                 .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
                                 .build();

    std::vector<VkDescriptorSet> global_descriptor_sets(Swapchain::MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < global_descriptor_sets.size(); ++i)
    {
        auto buffer_info = global_ubo_buffers[i]->getDescriptorInfo();
        auto image_info = image.getDescriptorInfo(sampler);

        DescriptorWriter(*global_set_layout, *globalPool)
            .writeBuffer(0, buffer_info)
            .writeImage(1, image_info)
            .build(global_descriptor_sets[i]);
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
            ubo.inverseViewMatrix = camera.getInverseViewMatrix();

            point_light_system.update(frame_info, ubo);

            global_ubo_buffers[current_frame_index]->write((void *)&ubo, sizeof(ubo));

            // Render
            renderer->beginRenderPass(command_buffer);

            // Order matters!
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
                     .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Swapchain::MAX_FRAMES_IN_FLIGHT)
                     .build();
}

void vk::App::loadObjects()
{
    {
        std::shared_ptr<Model> skull_model = std::make_shared<Model>(*device);
        if (!skull_model->loadFromFile("assets/models/skull.obj"))
            throw std::runtime_error("vl::App::loadObjects: Failed to load Skull model");

        Object skull;
        skull.setModel(skull_model);
        skull.setTranslation({0.f, 0.f, 0.f});
        skull.setScale({.05f, .05f, .05f});
        skull.setRotation({Angle::Rad90, 0.f, 0.f});
        objects[skull.getId()] = std::move(skull);
    }

    std::vector<Color> light_colors{COLOR_RED,  COLOR_ORANGE, COLOR_YELLOW, COLOR_GREEN,
                                    COLOR_CYAN, COLOR_BLUE,   COLOR_PURPLE};

    for (int i = 0; i < light_colors.size(); i++)
    {
        Object point_light = Object::makePointLight(.8f);

        point_light.setColor(light_colors[i]);

        auto rotate_light =
            Matrix::rotate(Matrix::identityMat4f(), (i * Angle::Rad360) / light_colors.size(), {0.f, -1.f, 0.f});

        point_light.setTranslation(Vec3f(rotate_light * Vec4f(-1.5f, -1.f, -1.5f, 1.f)));
        objects[point_light.getId()] = std::move(point_light);
    }
}
