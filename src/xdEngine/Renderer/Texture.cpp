#include <gli/gli.hpp>

#include "Texture.hpp"
#include "xdCore.hpp"


TextureWorker::TextureWorker() : device(nullptr), result(vk::Result::eNotReady)
{

}

void TextureWorker::CreateTextureImage()
{
    int texWidth = 130, texHeight = 130, texChannels;

    gli::texture2d tex2D(gli::load(Core.TexturesPath.string() + "texture.dds"));
    //stbi_uc* pixels = stbi_load("textures/texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    vk::DeviceSize imageSize = texWidth * texHeight * 4;

    //if (!pixels)
    //throw std::runtime_error("failed to load texture image!");

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;

    createBuffer(imageSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        stagingBuffer, stagingBufferMemory);

    auto data = device.mapMemory(stagingBufferMemory, 0, imageSize);
    memcpy(data, tex2D.data(), static_cast<size_t>(imageSize));
    device.unmapMemory(stagingBufferMemory);

    //stbi_image_free(pixels);

    createImage(texWidth, texHeight,
        vk::Format::eR8G8B8A8Unorm, vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
        vk::MemoryPropertyFlagBits::eDeviceLocal, textureImage, textureImageMemory);

    transitionImageLayout(textureImage, vk::Format::eR8G8B8A8Unorm,
        vk::ImageLayout::ePreinitialized, vk::ImageLayout::eTransferDstOptimal);

    copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

    transitionImageLayout(textureImage, vk::Format::eR8G8B8A8Unorm,
        vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

    device.destroyBuffer(stagingBuffer);
    device.freeMemory(stagingBufferMemory);
}

void TextureWorker::CreateTextureImageView()
{
    textureImageView = createImageView(textureImage, vk::Format::eR8G8B8A8Unorm);
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
    vk::MemoryAllocateInfo allocInfo(memRequirements.size, findMemoryType(memRequirements.memoryTypeBits, properties));

    imageMemory = device.allocateMemory(allocInfo);
    assert(imageMemory);

    device.bindImageMemory(image, imageMemory, 0);
}

vk::ImageView TextureWorker::createImageView(vk::Image image, vk::Format format)
{
    vk::ImageViewCreateInfo viewInfo({}, image, vk::ImageViewType::e2D, format);
    viewInfo.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
    viewInfo.subresourceRange.setLevelCount(1);
    viewInfo.subresourceRange.setLayerCount(1);

    auto imageView = device.createImageView(viewInfo);

    return imageView;
}