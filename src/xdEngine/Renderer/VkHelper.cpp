#include "Renderer.hpp"
#include "VkHelper.hpp"
#include "Debug/Log.hpp"

using namespace XDay;

vk_helper VkHelper;

vk_helper::vk_helper()
{
    physDevice = Render.physDevice;
    device = Render.device;
    commandPool = Render.commandPool;
    graphicsQueue = Render.graphicsQueue;
}

void vk_helper::SetPhysicalDevice(vk::PhysicalDevice _physDevice)
{
    physDevice = _physDevice;
}

void vk_helper::SetDevice(vk::Device _device)
{
    device = _device;
}

void vk_helper::SetCommandPool(vk::CommandPool _commandPool)
{
    commandPool = _commandPool;
}

void vk_helper::SetGraphicsQueue(vk::Queue _graphicsQueue)
{
    graphicsQueue = _graphicsQueue;
}

uint32_t vk_helper::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
{
    vk::PhysicalDeviceMemoryProperties memProperties = physDevice.getMemoryProperties();

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;

    Log("Renderer::findMemoryType():: failed to find suitable memory type!");
    throw std::runtime_error("failed to find suitable memory type!");
}

vk::CommandBuffer vk_helper::beginSingleTimeCommands()
{
    vk::CommandBufferAllocateInfo allocInfo(commandPool, vk::CommandBufferLevel::ePrimary, 1);

    vk::CommandBuffer commandBuffer;
    device.allocateCommandBuffers(&allocInfo, &commandBuffer);

    vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    commandBuffer.begin(beginInfo);

    return commandBuffer;
}

void vk_helper::endSingleTimeCommands(vk::CommandBuffer commandBuffer)
{
    commandBuffer.end();

    vk::SubmitInfo submitInfo;
    submitInfo.setCommandBufferCount(1);
    submitInfo.setPCommandBuffers(&commandBuffer);

    graphicsQueue.submit(1, &submitInfo, nullptr);
    graphicsQueue.waitIdle();
    device.freeCommandBuffers(commandPool, 1, &commandBuffer);
}

void vk_helper::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory)
{
    vk::BufferCreateInfo bufferInfo({}, size, usage, vk::SharingMode::eExclusive);

    buffer = device.createBuffer(bufferInfo);

    auto memRequirements = device.getBufferMemoryRequirements(buffer);

    vk::MemoryAllocateInfo allocInfo(memRequirements.size, findMemoryType(memRequirements.memoryTypeBits, properties));

    bufferMemory = device.allocateMemory(allocInfo);

    device.bindBufferMemory(buffer, bufferMemory, 0);
}

void vk_helper::copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
{
    vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

    vk::BufferCopy copyRegion(0, 0, size);

    commandBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer);
}

vk::ImageView vk_helper::createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags)
{
    vk::ImageSubresourceRange range(aspectFlags, 0, 1, 0, 1);
    vk::ImageViewCreateInfo viewInfo({}, image, vk::ImageViewType::e2D, format);
    viewInfo.setSubresourceRange(range);

    auto imageView = device.createImageView(viewInfo);

    return imageView;
}
