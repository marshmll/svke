#include "System/Core/Device.hpp"

fl::Device::Device(Window &window) : window(window)
{
    nullifyHandles();
    createInstance();
    setupDebugMessenger();
    createSurface();
    pickAdequatePhysicalDevice();
    createLogicalDevice();
    createVmaAllocator();
    createCommandPool();
}

fl::Device::~Device()
{
    vkDestroyCommandPool(device, commandPool, nullptr);
    vmaDestroyAllocator(allocator);
    vkDestroyDevice(device, nullptr);

#ifndef NDEBUG
    destroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
#endif

    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
}

VmaAllocator fl::Device::getAllocator()
{
    return allocator;
}

const VkPhysicalDeviceProperties &fl::Device::getProperties() const
{
    return properties;
}

VkDevice fl::Device::getLogicalDevice()
{
    return device;
}

VkSurfaceKHR fl::Device::getSurface()
{
    return surface;
}

VkCommandPool fl::Device::getCommandPool()
{
    return commandPool;
}

VkQueue fl::Device::getGraphicsQueue()
{
    return graphicsQueue;
}

VkQueue fl::Device::getPresentQueue()
{
    return presentQueue;
}

fl::Device::SwapchainSupportDetails fl::Device::getSwapchainSupport()
{
    return querySwapchainSupport(physicalDevice);
}

const uint32_t fl::Device::findMemoryType(const uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &mem_properties);

    for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    throw std::runtime_error("fl::Device::findMemoryType: FAILED TO FIND MEMORY TYPE");
}

fl::Device::QueueFamilyIndices fl::Device::findPhysicalQueueFamilies()
{
    return findQueueFamilies(physicalDevice);
}

VkFormat fl::Device::findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling,
                                         VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
            return format;

        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
            return format;
    }

    throw std::runtime_error("fl::Device::findSupportedFormat: FAILED TO FIND SUPPORTED FORMAT");
}

void fl::Device::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memory_usage,
                              VkBuffer &buffer, VmaAllocation &allocation)
{
    VkBufferCreateInfo buffer_info = {};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usage;

    VmaAllocationCreateInfo alloc_info = {};
    alloc_info.usage = memory_usage;

    if (vmaCreateBuffer(allocator, &buffer_info, &alloc_info, &buffer, &allocation, nullptr) != VK_SUCCESS)
        throw std::runtime_error("fl::Device::createBuffer: FAILED TO CREATE BUFFER");
}

void fl::Device::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memory_usage,
                              VkBuffer &buffer, VmaAllocation &allocation, VmaAllocationCreateFlags flags)
{
    VkBufferCreateInfo buffer_info = {};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usage;

    VmaAllocationCreateInfo alloc_info = {};
    alloc_info.usage = memory_usage;
    alloc_info.flags = flags;

    if (vmaCreateBuffer(allocator, &buffer_info, &alloc_info, &buffer, &allocation, nullptr) != VK_SUCCESS)
        throw std::runtime_error("fl::Device::createBuffer: FAILED TO CREATE BUFFER");
}

VkCommandBuffer fl::Device::beginSingleTimeCommands()
{
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = commandPool;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer;
    vkAllocateCommandBuffers(device, &alloc_info, &command_buffer);

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(command_buffer, &begin_info);
    return command_buffer;
}

void fl::Device::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void fl::Device::copyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size)
{
    VkCommandBuffer command_buffer = beginSingleTimeCommands();

    VkBufferCopy copy_region = {};
    copy_region.srcOffset = 0;
    copy_region.dstOffset = 0;
    copy_region.size = size;
    vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

    endSingleTimeCommands(command_buffer);
}

void fl::Device::copyBufferToImage(VkBuffer &buffer, VkImage &image, const uint32_t width, const uint32_t height,
                                   const uint32_t layer_count)
{
    VkCommandBuffer command_buffer = beginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = layer_count;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {width, height, 1};

    vkCmdCopyBufferToImage(command_buffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    endSingleTimeCommands(command_buffer);
}

void fl::Device::createImageWithInfo(const VkImageCreateInfo &image_info, VkMemoryPropertyFlags properties,
                                     VkImage &image, VmaAllocation &image_memory)
{
    // Define VMA allocation create info
    VmaAllocationCreateInfo alloc_create_info = {};
    alloc_create_info.usage = VMA_MEMORY_USAGE_AUTO; // Let VMA decide the best memory type
    alloc_create_info.requiredFlags = properties;    // Enforce the memory properties (e.g., device-local, host-visible)

    // Create the image using VMA
    VkResult result = vmaCreateImage(allocator,          // VmaAllocator instance
                                     &image_info,        // Image create info
                                     &alloc_create_info, // Allocation create info
                                     &image,             // Output: VkImage
                                     &image_memory,      // Output: VmaAllocation
                                     nullptr             // Optional: Output: VmaAllocationInfo
    );

    if (result != VK_SUCCESS)
        throw std::runtime_error("fl::Device::createImageWithInfo: FAILED TO CREATE IMAGE WITH VMA");
}

void fl::Device::nullifyHandles()
{
    instance = VK_NULL_HANDLE;
    debugMessenger = VK_NULL_HANDLE;
    surface = VK_NULL_HANDLE;
    physicalDevice = VK_NULL_HANDLE;
    device = VK_NULL_HANDLE;
    graphicsQueue = VK_NULL_HANDLE;
    presentQueue = VK_NULL_HANDLE;
    allocator = VK_NULL_HANDLE;
    commandPool = VK_NULL_HANDLE;
}

void fl::Device::createInstance()
{
#ifndef NDEBUG
    if (!checkValidationLayerSupport())
        throw std::runtime_error("fl::Device::createInstance: VALIDATION LAYERS REQUESTED, BUT NOT AVALIABLE.");
#endif

    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "Flare";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "No Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

    auto extensions = getRequiredExtensions();
    create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    create_info.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info;

#ifndef NDEBUG
    create_info.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
    create_info.ppEnabledLayerNames = VALIDATION_LAYERS.data();

    populateDebugMessengerCreateInfo(debug_create_info);
    create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debug_create_info;
#else
    create_info.enabledLayerCount = 0;
    create_info.pNext = nullptr;
#endif

    if (vkCreateInstance(&create_info, nullptr, &instance) != VK_SUCCESS)
        throw std::runtime_error("fl::Device::createInstance: FAILED TO CREATE VULKAN INSTANCE");

    checkGflwRequiredInstanceExtensions();
}

void fl::Device::setupDebugMessenger()
{
#ifndef NDEBUG
    VkDebugUtilsMessengerCreateInfoEXT create_info;

    populateDebugMessengerCreateInfo(create_info);

    if (createDebugUtilsMessengerEXT(instance, &create_info, nullptr, &debugMessenger) != VK_SUCCESS)
        throw std::runtime_error("fl::Device::setupDebugMessenger: FAILED TO SETUP DEBUG MESSENGER");
#endif
}

void fl::Device::createSurface()
{
    window.createSurface(instance, surface);
}

void fl::Device::pickAdequatePhysicalDevice()
{
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, nullptr);

    if (device_count == 0)
        throw std::runtime_error("fl::Device::pickAdequatePhysicalDevice: NO GPU(S) WITH VULKAN SUPPORT FOUND");

    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

    std::multimap<int, VkPhysicalDevice> candidate_devices;

    for (auto &device : devices)
    {
        int score = rateDeviceSuitability(device);
        candidate_devices.insert(std::make_pair(score, device));
    }

    if (candidate_devices.rbegin()->first >= 0)
        physicalDevice = candidate_devices.rbegin()->second;
    else
        throw std::runtime_error("fl::Device::pickAdequatePhysicalDevice: FAILED TO FIND A SUITABLE GPU");

    vkGetPhysicalDeviceProperties(physicalDevice, &properties);

#ifndef NDEBUG
    std::cout << "SELECTED DEVICE: " << properties.deviceName << std::endl;
#endif
}

void fl::Device::createLogicalDevice()
{
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {*indices.graphicsFamily, *indices.presentFamily};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(DEVICE_EXTENSIONS.size());
    createInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();

    // might not really be necessary anymore because device specific validation layers
    // have been deprecated
#ifndef NDEBUG
    createInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
    createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
#else
    createInfo.enabledLayerCount = 0;
#endif

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
    {
        throw std::runtime_error("fl::Device::createLogicalDevice: FAILED TO CREATE LOGICAL DEVICE");
    }

    vkGetDeviceQueue(device, *indices.graphicsFamily, 0, &graphicsQueue);
    vkGetDeviceQueue(device, *indices.presentFamily, 0, &presentQueue);
}

void fl::Device::createVmaAllocator()
{
    // Fill the VmaAllocatorCreateInfo structure
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_3;
    allocatorInfo.instance = instance;
    allocatorInfo.physicalDevice = physicalDevice;
    allocatorInfo.device = device;

    // Optional: Enable VMA's recording of statistics (useful for debugging)
    allocatorInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;

    // Create the VMA allocator
    if (vmaCreateAllocator(&allocatorInfo, &allocator) != VK_SUCCESS)
        throw std::runtime_error("fl::Device::createVmaAllocator: FAILED TO CREATE VMA ALLOCATOR");
}

void fl::Device::createCommandPool()
{
    QueueFamilyIndices queue_family_indices = findPhysicalQueueFamilies();

    VkCommandPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = *queue_family_indices.graphicsFamily;
    pool_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(device, &pool_info, nullptr, &commandPool) != VK_SUCCESS)
        throw std::runtime_error("fl::Device::createCommandPool FAILED TO CREATE COMMAND POOL");
}

const bool fl::Device::isDeviceSuitable(VkPhysicalDevice physical_device)
{
    QueueFamilyIndices indices = findQueueFamilies(physical_device);

    bool extensions_supported = checkDeviceExtensionSupport(physical_device);
    bool swapchain_adequate = false;

    if (extensions_supported)
    {
        SwapchainSupportDetails swapchain_support = querySwapchainSupport(physical_device);
        swapchain_adequate = !swapchain_support.formats.empty() && !swapchain_support.presentModes.empty();
    }

    VkPhysicalDeviceFeatures supported_features;
    vkGetPhysicalDeviceFeatures(physical_device, &supported_features);

    return indices.isComplete() && extensions_supported && swapchain_adequate && supported_features.samplerAnisotropy;
}

const int fl::Device::rateDeviceSuitability(VkPhysicalDevice physical_device)
{
    int score = 0;

    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceProperties(physical_device, &properties);
    vkGetPhysicalDeviceFeatures(physical_device, &features);
    bool extension_support = checkDeviceExtensionSupport(physical_device);

    // Prefer discrete GPU over integrated GPU.
    if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        score += 10;

    else if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        score += 1;

    // Max possible texture size to max graphics quality.
    score += properties.limits.maxImageDimension2D;

    // Require geometry shader.
    if (!features.geometryShader)
        score = -1;

    // Require sampler anisotropy.
    if (!features.samplerAnisotropy)
        score = -1;

    // Require a device with a valid queue familiy.
    if (!findQueueFamilies(physical_device).isComplete())
        score = -1;

    // Require that swap chain is adequate
    if (extension_support)
    {
        SwapchainSupportDetails details = querySwapchainSupport(physical_device);
        if (details.formats.empty() || details.presentModes.empty())
            score = -1;
    }

    // Require a device with extensions support.
    if (!extension_support)
        score = -1;

    return score;
}

const std::vector<const char *> fl::Device::getRequiredExtensions()
{
    uint32_t glfw_extension_count = 0;
    const char **glfw_extensions;

    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    std::vector<const char *> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

#ifndef NDEBUG
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    return std::move(extensions);
}

const bool fl::Device::checkValidationLayerSupport()
{
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    std::vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    for (auto *layer_name : VALIDATION_LAYERS)
    {
        bool layer_found = false;

        for (const auto &layer_properties : available_layers)
        {
            if (strcmp(layer_name, layer_properties.layerName) == 0)
            {
                layer_found = true;
                break;
            }
        }

        if (!layer_found)
            return false;
    }

    return true;
}

void fl::Device::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &create_info)
{
    create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    create_info.pfnUserCallback = fl::Device::debugCallback;
    create_info.pUserData = nullptr;
}

void fl::Device::checkGflwRequiredInstanceExtensions()
{
    // Get the number of available extensions
    uint32_t extension_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);

    // Retrieve the available extensions
    std::vector<VkExtensionProperties> extensions(extension_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());

    // Store available extension names in a set of strings
    std::set<std::string> available_extensions;
#ifndef NDEBUG
    std::cout << "AVAILABLE EXTENSIONS:" << std::endl;
    for (const auto &extension : extensions)
    {
        std::cout << "    " << extension.extensionName << std::endl;
        available_extensions.insert(extension.extensionName);
    }
#else
    for (const auto &extension : extensions)
        available_extensions.insert(extension.extensionName);
#endif

    // Get the required extensions
    auto required_extensions = getRequiredExtensions();
#ifndef NDEBUG
    std::cout << "REQUIRED EXTENSIONS:" << std::endl;
    for (const auto &required : required_extensions)
    {
        std::cout << "    " << required << std::endl;

        // Check if the required extension is in the available extensions set
        if (available_extensions.find(required) == available_extensions.end())
        {
            throw std::runtime_error(
                std::string("fl::Device::checkGflwRequiredInstanceExtensions: MISSING REQUIRED GLFW EXTENSION: ") +
                required);
        }
    }
#else
    for (const auto &required : required_extensions)
    {
        // Check if the required extension is in the available extensions set
        if (available_extensions.find(required) == available_extensions.end())
        {
            throw std::runtime_error(
                std::string("fl::Device::checkGflwRequiredInstanceExtensions: MISSING REQUIRED GLFW EXTENSION: ") +
                required);
        }
    }
#endif
}

const bool fl::Device::checkDeviceExtensionSupport(VkPhysicalDevice physical_device)
{
    uint32_t extension_count = 0;
    vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, nullptr);

    std::vector<VkExtensionProperties> available_extensions(extension_count);
    vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, available_extensions.data());

    std::set<std::string> required_extensions(DEVICE_EXTENSIONS.begin(), DEVICE_EXTENSIONS.end());

    for (const auto &extension : available_extensions)
        required_extensions.erase(extension.extensionName);

    return required_extensions.empty();
}

fl::Device::QueueFamilyIndices fl::Device::findQueueFamilies(VkPhysicalDevice physical_device)
{
    QueueFamilyIndices indices;

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);

    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families.data());

    int i = 0;
    for (const auto &queue_family : queue_families)
    {
        if (queue_family.queueCount > 0 && queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices.graphicsFamily = i;

        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &present_support);

        if (queue_family.queueCount > 0 && present_support)
            indices.presentFamily = i;

        if (indices.isComplete())
            break;

        i++;
    }

    return indices;
}

fl::Device::SwapchainSupportDetails fl::Device::querySwapchainSupport(VkPhysicalDevice physical_device)
{
    SwapchainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &details.capabilities);

    uint32_t format_count;

    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, nullptr);

    if (format_count != 0)
    {
        details.formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, details.formats.data());
    }

    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, nullptr);

    if (present_mode_count != 0)
    {
        details.presentModes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count,
                                                  details.presentModes.data());
    }

    return details;
}

VKAPI_ATTR VkBool32 VKAPI_CALL fl::Device::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                         VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                         const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                         void *pUserData)
{
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
        std::cout << "[ VL VERB ] " << pCallbackData->pMessage << std::endl;

    else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        std::cout << "[ VL INFO ] " << pCallbackData->pMessage << std::endl;

    else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        std::cout << "[ VL WARN ] " << pCallbackData->pMessage << std::endl;

    else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        std::cerr << "[ VL ERROR ] " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

VkResult fl::Device::createDebugUtilsMessengerEXT(VkInstance instance,
                                                  const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                                  const VkAllocationCallbacks *pAllocator,
                                                  VkDebugUtilsMessengerEXT *pDebugMessenger)
{
    auto fn = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

    if (fn != nullptr)
        return fn(instance, pCreateInfo, pAllocator, pDebugMessenger);

    else
        return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void fl::Device::destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                               const VkAllocationCallbacks *pAllocator)
{
    auto fn = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

    if (fn != nullptr)
        fn(instance, debugMessenger, pAllocator);
}
