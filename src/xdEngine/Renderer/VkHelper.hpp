#pragma once
#ifndef Buffer_hpp__
#define Buffer_hpp__
#include <vulkan/vulkan.hpp>

namespace XDay
{

class vk_helper
{
    vk::PhysicalDevice& physDevice;
    vk::Device& device;
    vk::CommandPool& commandPool;
    vk::Queue& graphicsQueue;

public:
    void SetPhysicalDevice(const vk::PhysicalDevice& _physDevice) const;
    void SetDevice(const vk::Device& _device) const;
    void SetCommandPool(const vk::CommandPool& _commandPool) const;
    void SetGraphicsQueue(const vk::Queue& _graphicsQueue) const;

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