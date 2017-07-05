#pragma once
#ifndef Texture_hpp__
#define Texture_hpp__
#include <string>

#include "Common/Platform.hpp" // this must be first
#include <vulkan/vulkan.hpp>


class TextureWorker
{
    std::string textureName;
    const vk::Device& device;

    vk::Image textureImage;
    vk::DeviceMemory textureImageMemory;
    vk::ImageView textureImageView;
    vk::Sampler textureSampler;

    vk::Result result;

public:
    TextureWorker();

    void CreateTextureImage();
    void CreateTextureImageView();
    void CreateTextureSampler();

    void createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& imageMemory);
    vk::ImageView createImageView(vk::Image image, vk::Format format);
};

#endif // Texture_hpp__