#include "SVKE/Core/Graphics/Shader.hpp"

vk::Shader::Shader(Device &device, const std::string &path) : device(device)
{
    SPIRVBinary spirv_bin = readShaderFile(path);

    VkShaderModuleCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = spirv_bin.size();
    create_info.pCode = spirv_bin.data();

    if (vkCreateShaderModule(device.getLogicalDevice(), &create_info, nullptr, &module) != VK_SUCCESS)
        throw std::runtime_error("vk::Shader::Shader: FAILED TO CREATE SHADER MODULE");
}

vk::Shader::~Shader()
{
    vkDestroyShaderModule(device.getLogicalDevice(), module, nullptr);
}

VkShaderModule &vk::Shader::getModule()
{
    return module;
}

vk::Shader::SPIRVBinary vk::Shader::readShaderFile(const std::string &path)
{
    std::ifstream file(path, std::ios::ate | std::ios::binary);

    if (!file.is_open())
        throw std::runtime_error("vk::Shader::readShaderFile: FAILED TO OPEN SHADER FILE: " + path);

    size_t filesize = file.tellg();
    SPIRVBinary content(filesize);

    file.seekg(0);
    file.read(reinterpret_cast<char *>(content.data()), filesize);
    file.close();
#ifndef NDEBUG
    std::cout << "LOADED SHADER " << path << " (" << filesize << " bytes)" << std::endl;
#endif

    return std::move(content);
}
