#include "SVKE/Core/Graphics/TextureImage.hpp"

vk::TextureImage::TextureImage(Device &device, Texture &texture, const Format &format, const Tiling &tiling,
                               const VkImageUsageFlags usage)
    : device(device), format(static_cast<VkFormat>(format))
{
    createImage(texture, static_cast<VkImageTiling>(tiling), usage);
    transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyTextureToImage(texture);
    transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    createImageView();
}

vk::TextureImage::~TextureImage()
{
    vkDestroyImageView(device.getLogicalDevice(), imageView, nullptr);
    vmaDestroyImage(device.getAllocator(), image, allocation);
}

VkDescriptorImageInfo vk::TextureImage::getDescriptorInfo(TextureSampler &sampler) const
{
    VkDescriptorImageInfo image_info{};
    image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    image_info.imageView = imageView;
    image_info.sampler = sampler.getSampler();

    return image_info;
}

void vk::TextureImage::createImage(Texture &texture, const VkImageTiling tiling, const VkImageUsageFlags usage)
{
    VkImageCreateInfo image_info{};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent.width = static_cast<uint32_t>(texture.getWidth());
    image_info.extent.height = static_cast<uint32_t>(texture.getHeight());
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.format = format;
    image_info.tiling = tiling;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = usage;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_info.flags = 0; // Optional

    // Define VMA allocation create info
    VmaAllocationCreateInfo alloc_create_info = {};
    alloc_create_info.usage = VMA_MEMORY_USAGE_AUTO;
    alloc_create_info.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
    alloc_create_info.priority = 1.0f;

    // Create the image using VMA
    VkResult result = vmaCreateImage(device.getAllocator(), // VmaAllocator instance
                                     &image_info,           // Image create info
                                     &alloc_create_info,    // Allocation create info
                                     &image,                // Output: VkImage
                                     &allocation,           // Output: VmaAllocation
                                     nullptr                // Optional: Output: VmaAllocationInfo
    );

    if (result != VK_SUCCESS)
        throw std::runtime_error("vk::TextureImage::Image: FAILED TO CREATE IMAGE");
}

void vk::TextureImage::transitionImageLayout(const VkImageLayout old_layout, const VkImageLayout new_layout)
{
    auto command_buffer = device.beginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = 0; // TODO
    barrier.dstAccessMask = 0; // TODO

    VkPipelineStageFlags source_stage;
    VkPipelineStageFlags destination_stage;

    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
        throw std::invalid_argument("vk::TextureImage::transitionImageLayout: UNSUPPORTED LAYOUT TRANSITION");
    }

    vkCmdPipelineBarrier(command_buffer, source_stage, destination_stage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    device.endSingleTimeCommands(command_buffer);
}

void vk::TextureImage::copyTextureToImage(Texture &texture)
{
    Buffer staging_buffer(device, texture.getSize(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_AUTO,
                          VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT |
                              VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

    staging_buffer.map();
    staging_buffer.write((void *)texture.getPixels(), texture.getSize());
    staging_buffer.unmap();

    auto command_buffer = device.beginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {static_cast<uint32_t>(texture.getWidth()), static_cast<uint32_t>(texture.getHeight()), 1};

    vkCmdCopyBufferToImage(command_buffer, staging_buffer.getBuffer(), image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                           &region);

    device.endSingleTimeCommands(command_buffer);
}

void vk::TextureImage::createImageView()
{
    VkImageViewCreateInfo view_info{};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image = image;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = VK_FORMAT_R8G8B8A8_SRGB;
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device.getLogicalDevice(), &view_info, nullptr, &imageView) != VK_SUCCESS)
        throw std::runtime_error("vk::TextureImage::createImageView: FAILED TO CREATE TEXTURE IMAGE VIEW");
}
