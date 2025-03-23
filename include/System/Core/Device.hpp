#pragma once

#include "System/Core/Window.hpp"

#include <vk_mem_alloc.h>

#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <set>
#include <map>
#include <optional>

namespace fl
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

    Device(Window &window);

    Device(const Device &) = delete;
    Device &operator=(const Device &) = delete;
    Device(Device &&) = delete;
    Device &operator=(Device &&) = delete;

    ~Device();

    VmaAllocator getAllocator();

    VkDevice getLogicalDevice();

    VkSurfaceKHR getSurface();

    VkCommandPool getCommandPool();

    VkQueue getGraphicsQueue();

    VkQueue getPresentQueue();

    SwapchainSupportDetails getSwapchainSupport();

    const uint32_t findMemoryType(const uint32_t typeFilter, VkMemoryPropertyFlags properties);

    QueueFamilyIndices findPhysicalQueueFamilies();

    VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling,
                                 VkFormatFeatureFlags features);

    // Buffer Helper Functions
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memory_usage, VkBuffer &buffer,
                      VmaAllocation &allocation);

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memory_usage, VkBuffer &buffer,
                      VmaAllocation &allocation, VmaAllocationCreateFlags flags);

    VkCommandBuffer beginSingleTimeCommands();

    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

    void copyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);

    void copyBufferToImage(VkBuffer &buffer, VkImage &image, const uint32_t width, const uint32_t height,
                           const uint32_t layer_count);

    void createImageWithInfo(const VkImageCreateInfo &image_info, VkMemoryPropertyFlags properties, VkImage &image,
                             VmaAllocation &image_memory);

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

    void nullifyHandles();

    void createInstance();

    void setupDebugMessenger();

    void createSurface();

    void pickAdequatePhysicalDevice();

    void createLogicalDevice();

    void createVmaAllocator();

    void createCommandPool();

    const bool isDeviceSuitable(VkPhysicalDevice physical_device);

    const int rateDeviceSuitability(VkPhysicalDevice physical_device);

    const std::vector<const char *> getRequiredExtensions();

    const bool checkValidationLayerSupport();

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

    void checkGflwRequiredInstanceExtensions();

    const bool checkDeviceExtensionSupport(VkPhysicalDevice physical_device);

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physical_device);

    SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice physical_device);

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
} // namespace fl
