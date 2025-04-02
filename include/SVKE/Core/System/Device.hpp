#pragma once

#include "SVKE/Core/System/Window.hpp"

#include <vk_mem_alloc.h>

#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <set>
#include <map>
#include <optional>

namespace vk
{
class Device
{
  public:
    inline static const std::vector<const char *> VALIDATION_LAYERS = {"VK_LAYER_KHRONOS_validation"};
    inline static const std::vector<const char *> DEVICE_EXTENSIONS = {VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                                                                       VK_EXT_MEMORY_BUDGET_EXTENSION_NAME};

    struct SwapchainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        inline const bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
    };

    enum class MSAA : int
    {
        x1 = VK_SAMPLE_COUNT_1_BIT,
        x2 = VK_SAMPLE_COUNT_2_BIT,
        x4 = VK_SAMPLE_COUNT_4_BIT,
        x8 = VK_SAMPLE_COUNT_8_BIT,
        x16 = VK_SAMPLE_COUNT_16_BIT,
        x32 = VK_SAMPLE_COUNT_32_BIT,
        x64 = VK_SAMPLE_COUNT_64_BIT
    };

    Device(Window &window, const MSAA &preferred_msaa_samples = MSAA::x1);

    Device(const Device &) = delete;
    Device &operator=(const Device &) = delete;
    Device(Device &&) = delete;
    Device &operator=(Device &&) = delete;

    ~Device();

    VmaAllocator getAllocator();

    const VkPhysicalDeviceProperties &getProperties() const;

    VkDevice getLogicalDevice();

    VkSurfaceKHR getSurface();

    VkCommandPool getCommandPool();

    VkQueue getGraphicsQueue();

    VkQueue getPresentQueue();

    SwapchainSupportDetails getSwapchainSupport();

    const VkSampleCountFlagBits &getMsaaMaxSamples() const;

    const VkSampleCountFlagBits &getCurrentMsaaSamples() const;

    VkSampleCountFlagBits getMsaaSamplesOrClosest(const MSAA &samples) const;

    QueueFamilyIndices findPhysicalQueueFamilies();

    VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling,
                                 VkFormatFeatureFlags features);

    VkCommandBuffer beginSingleTimeCommands();

    void endSingleTimeCommands(VkCommandBuffer command_buffer);

    void createImageWithInfo(const VkImageCreateInfo &image_info, VkMemoryPropertyFlags properties, VkImage &image,
                             VmaAllocation &image_memory);

    static const std::string getMsaaSamplesAsString(const MSAA &samples);

  private:
    Window &window;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice;
    VkPhysicalDeviceProperties properties;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VmaAllocator allocator;
    VkCommandPool commandPool;

    VkSampleCountFlagBits msaaMaxSamples;
    VkSampleCountFlagBits currentMsaaSamples;

    void nullifyHandles();

    void createInstance();

    void setupDebugMessenger();

    void createSurface();

    void pickAdequatePhysicalDevice(const MSAA &preferred_msaa_level);

    void createLogicalDevice();

    void createVmaAllocator();

    void createCommandPool();

    const int rateDeviceSuitability(VkPhysicalDevice physical_device);

    const std::vector<const char *> getRequiredExtensions();

    const bool checkValidationLayerSupport();

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

    void checkGflwRequiredInstanceExtensions();

    const bool checkDeviceExtensionSupport(VkPhysicalDevice physical_device);

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physical_device);

    SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice physical_device);

    VkSampleCountFlagBits queryMaxUsableSampleCount(VkPhysicalDevice physical_device);

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                        void *pUserData);

    static VkResult createDebugUtilsMessengerEXT(VkInstance instance,
                                                 const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                                 const VkAllocationCallbacks *pAllocator,
                                                 VkDebugUtilsMessengerEXT *pDebugMessenger);

    static void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                              const VkAllocationCallbacks *pAllocator);
};
} // namespace vk
