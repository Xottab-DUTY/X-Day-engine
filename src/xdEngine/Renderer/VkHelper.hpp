#pragma once
#ifndef Buffer_hpp__
#define Buffer_hpp__
#include <vulkan/vulkan.hpp>

namespace XDay
{

class vk_helper
{
    vk::PhysicalDevice physDevice = nullptr;
    vk::Device device = nullptr;
    vk::CommandPool commandPool = nullptr;
    vk::Queue graphicsQueue = nullptr;

public:
    vk_helper();

    void SetPhysicalDevice(vk::PhysicalDevice _physDevice);
    void SetDevice(vk::Device _device);
    void SetCommandPool(vk::CommandPool _commandPool);
    void SetGraphicsQueue(vk::Queue _graphicsQueue);

    vk::ImageView createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags);

    uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

    vk::CommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(vk::CommandBuffer commandBuffer);

    void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory);
    void copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);
};

}

extern XDay::vk_helper VkHelper;
#endif // Buffer_hpp__