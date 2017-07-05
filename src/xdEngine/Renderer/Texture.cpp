#include <vulkan/vulkan.hpp>
#include <gli/gli.hpp>

#include "Debug/Log.hpp"
#include "xdCore.hpp"
#include "Renderer.hpp"
#include "Texture.hpp"
#include "VkHelper.hpp"


using namespace XDay;

TextureWorker TextureHelper(Render.device);

TextureWorker::TextureWorker(const vk::Device& _device) : device(_device), result(vk::Result::eNotReady)
{

}

void TextureWorker::CreateTextureImage()
{
    gli::texture2d tex2D(gli::load(Core.TexturesPath.string() + "chalet.dds"));
    vk::DeviceSize imageSize = tex2D[0].extent().x * tex2D[0].extent().y * 4;

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;

    VkHelper.createBuffer(imageSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        stagingBuffer, stagingBufferMemory);

    auto data = device.mapMemory(stagingBufferMemory, 0, imageSize);
    memcpy(data, tex2D[0].data(), static_cast<size_t>(imageSize));
    device.unmapMemory(stagingBufferMemory);

    createImage(tex2D[0].extent().x, tex2D[0].extent().y,
        vk::Format::eA8B8G8R8UnormPack32, vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
        vk::MemoryPropertyFlagBits::eDeviceLocal, textureImage, textureImageMemory);

    transitionImageLayout(textureImage, vk::Format::eA8B8G8R8UnormPack32,
        vk::ImageLayout::ePreinitialized, vk::ImageLayout::eTransferDstOptimal);

    copyBufferToImage(stagingBuffer, textureImage, tex2D[0].extent().x, tex2D[0].extent().y);

    transitionImageLayout(textureImage, vk::Format::eA8B8G8R8UnormPack32,
        vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

    device.destroyBuffer(stagingBuffer);
    device.freeMemory(stagingBufferMemory);
}

void TextureWorker::CreateTextureImageView()
{
    textureImageView = VkHelper.createImageView(textureImage, vk::Format::eR8G8B8A8Unorm, vk::ImageAspectFlagBits::eColor);
}

void TextureWorker::CreateTextureSampler()
{
    vk::SamplerCreateInfo samplerInfo({},
        vk::Filter::eLinear, vk::Filter::eLinear);
    samplerInfo.setAddressModeU(vk::SamplerAddressMode::eRepeat);
    samplerInfo.setAddressModeV(vk::SamplerAddressMode::eRepeat);
    samplerInfo.setAddressModeW(vk::SamplerAddressMode::eRepeat);
    samplerInfo.setAnisotropyEnable(VK_TRUE);
    samplerInfo.setMaxAnisotropy(16);
    samplerInfo.setBorderColor(vk::BorderColor::eIntOpaqueBlack);
    samplerInfo.setUnnormalizedCoordinates(VK_FALSE);
    samplerInfo.setCompareEnable(VK_FALSE);
    samplerInfo.setCompareOp(vk::CompareOp::eAlways);
    samplerInfo.setMipmapMode(vk::SamplerMipmapMode::eLinear);

    textureSampler = device.createSampler(samplerInfo);
    assert(textureSampler);
}

void TextureWorker::createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling,
    vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& imageMemory)
{
    vk::ImageCreateInfo imageInfo({}, vk::ImageType::e2D, format);
    imageInfo.extent.setWidth(width);
    imageInfo.extent.setHeight(height);
    imageInfo.extent.setDepth(1);
    imageInfo.setMipLevels(1);
    imageInfo.setArrayLayers(1);
    imageInfo.setTiling(tiling);
    imageInfo.setInitialLayout(vk::ImageLayout::ePreinitialized);
    imageInfo.setUsage(usage);
    imageInfo.setSamples(vk::SampleCountFlagBits::e1);
    imageInfo.setSharingMode(vk::SharingMode::eExclusive);

    image = device.createImage(imageInfo);
    assert(image);

    auto memRequirements = device.getImageMemoryRequirements(image);
    vk::MemoryAllocateInfo allocInfo(memRequirements.size, VkHelper.findMemoryType(memRequirements.memoryTypeBits, properties));

    imageMemory = device.allocateMemory(allocInfo);
    assert(imageMemory);

    device.bindImageMemory(image, imageMemory, 0);
}

void TextureWorker::transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
    vk::CommandBuffer commandBuffer = VkHelper.beginSingleTimeCommands();

    vk::ImageSubresourceRange range(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);

    if (newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
    {
        range.setAspectMask(vk::ImageAspectFlagBits::eDepth);

        if (hasStencilComponent(format))
            range.aspectMask |= vk::ImageAspectFlagBits::eStencil;
    }

    vk::ImageMemoryBarrier barrier({}, {}, oldLayout, newLayout, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, image, range);

    if (oldLayout == vk::ImageLayout::ePreinitialized && newLayout == vk::ImageLayout::eTransferSrcOptimal)
    {
        barrier.setSrcAccessMask(vk::AccessFlagBits::eHostWrite);
        barrier.setDstAccessMask(vk::AccessFlagBits::eTransferRead);
    }
    else if (oldLayout == vk::ImageLayout::ePreinitialized && newLayout == vk::ImageLayout::eTransferDstOptimal)
    {
        barrier.setSrcAccessMask(vk::AccessFlagBits::eHostWrite);
        barrier.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);
    }
    else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
    {
        barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
        barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);
    }
    else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
    {
        barrier.setSrcAccessMask({});
        barrier.setDstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite);
    }
    else
    {
        Log("Renderer::transitionImageLayout():: unsupported layout transition!");
        throw std::invalid_argument("unsupported layout transition!");
    }


    commandBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTopOfPipe,
        {},
        0, nullptr,
        0, nullptr,
        1, &barrier);

    VkHelper.endSingleTimeCommands(commandBuffer);
}

void TextureWorker::copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height)
{
    vk::CommandBuffer commandBuffer = VkHelper.beginSingleTimeCommands();

    vk::ImageSubresourceLayers subres(vk::ImageAspectFlagBits::eColor, 0, 0, 1);
    vk::BufferImageCopy region(0, 0, 0, subres, { 0, 0, 0 }, { width, height, 1 });

    commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &region);

    VkHelper.endSingleTimeCommands(commandBuffer);
}

bool TextureWorker::hasStencilComponent(vk::Format format) const
{
    return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}
