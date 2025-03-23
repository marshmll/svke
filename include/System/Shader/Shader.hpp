#pragma once

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif

#include <GLFW/glfw3.h>
#include <shaderc/shaderc.hpp>

#include <System/Core/Device.hpp>

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>

namespace fl
{
class Shader
{
  public:
    using SPIRVBinary = std::vector<uint32_t>;

    Shader(Device &device, const std::string &path);
    Shader(const Shader &) = delete;
    Shader &operator=(const Shader &) = delete;

    ~Shader();

    VkShaderModule &getModule();

  private:
    Device &device;
    VkShaderModule module;

    SPIRVBinary readShaderFile(const std::string &path);
};
} // namespace fl
