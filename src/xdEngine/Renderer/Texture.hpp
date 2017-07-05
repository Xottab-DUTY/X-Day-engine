#pragma once
#ifndef Texture_hpp__
#define Texture_hpp__
#include <string>

#include "Common/Platform.hpp" // this must be first
#include <vulkan/vulkan.hpp>

namespace XDay
{

class TextureWorker
{
    std::string textureName;
    const vk::Device& device;

    vk::Image textureImage;
    vk::DeviceMemory textureImageMemory;
    

    vk::Result result;

public:
    vk::ImageView textureImageView;
    vk::Sampler textureSampler;

    TextureWorker(const vk::Device& _device);

    void CreateTextureImage();
    void CreateTextureImageView();
    void CreateTextureSampler();

    void createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& imageMemory);

    void transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
    void copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);

    bool hasStencilComponent(vk::Format format) const;
};
}

extern XDay::TextureWorker TextureHelper;
#endif // Texture_hpp__