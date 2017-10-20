#pragma once
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

#include "Common/import_export_macros.hpp"
#include <ResourceLimits.h>

class VkDemoRenderer
{
public:
    vk::UniqueInstance vkInstance;
    vk::UniqueDebugReportCallbackEXT vkCallback;
    vk::UniqueSurfaceKHR surface;

    vk::PhysicalDevice physDevice;
    vk::UniqueDevice device;

    vk::Queue graphicsQueue;
    vk::Queue presentQueue;

    vk::UniqueSwapchainKHR swapchain;
    std::vector<vk::Image> swapChainImages;
    vk::Format swapChainImageFormat;
    vk::Extent2D swapChainExtent;
    std::vector<vk::UniqueImageView> swapChainImageViews;
    std::vector<vk::UniqueFramebuffer> swapChainFramebuffers;

    vk::UniqueRenderPass renderPass;
    vk::UniqueDescriptorSetLayout descriptorSetLayout;
    vk::UniquePipelineLayout pipelineLayout;
    vk::UniquePipeline graphicsPipeline;

    vk::UniqueCommandPool commandPool;

    vk::UniqueImage depthImage;
    vk::UniqueDeviceMemory depthImageMemory;
    vk::UniqueImageView depthImageView;

    vk::UniqueImage textureImage;
    vk::UniqueDeviceMemory textureImageMemory;
    vk::UniqueImageView textureImageView;
    vk::UniqueSampler textureSampler;

    vk::UniqueBuffer vertexBuffer;
    vk::UniqueDeviceMemory vertexBufferMemory;

    vk::UniqueBuffer indexBuffer;
    vk::UniqueDeviceMemory indexBufferMemory;

    vk::UniqueBuffer uniformBuffer;
    vk::UniqueDeviceMemory uniformBufferMemory;

    vk::UniqueDescriptorPool descriptorPool;
    vk::DescriptorSet descriptorSet;

    std::vector<vk::UniqueCommandBuffer> commandBuffers;
    vk::UniqueFence imageAvailableFence;
    vk::UniqueSemaphore renderFinishedSemaphore;

    vk::Result result;

    TBuiltInResource resources;

    struct QueueFamilyIndices
    {
        int graphicsFamily = -1;
        int presentFamily = -1;

        bool isComplete() const;
    };

    struct SwapChainSupportDetails
    {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };

    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 texCoord;

        static vk::VertexInputBindingDescription getBindingDescription();
        static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescriptions();

        bool operator==(const Vertex& other) const;
    };

    struct UniformBufferObject
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    bool renderPaused;

    VkDemoRenderer();

    void Initialize();
    void UpdateUniformBuffer();
    void DrawFrame();

    void RecreateSwapChain();

private:
    void InitializeResources();
    void CreateVkInstance();
    bool CheckValidationLayersSupport() const;
    std::vector<const char*> getRequiredExtensions();
    void CreateDebugCallback();

    void CreateVkSurface();

    void GetPhysDevice();
    bool isPhysDeviceSuitable(vk::PhysicalDevice _physDevice) const;

    bool checkDeviceExtensionSupport(vk::PhysicalDevice _physDevice) const;
    void CreateDevice();

    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) const;
    vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes) const;
    vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const;
    void CreateSwapChain();
    void CreateImageViews();
    void CreateRenderPass();
    void CreateDescriptorSetLayout();
    void CreateGraphicsPipeline();
    void CreateFramebuffers();
    void CreateCommandPool();
    void CreateDepthResources();
    void CreateTextureImage();
    void CreateTextureImageView();
    void CreateTextureSampler();
    void LoadModel();
    void CreateVertexBuffer();
    void CreateIndexBuffer();
    void CreateUniformBuffer();
    void CreateDescriptorPool();
    void CreateDescriptorSet();
    void CreateCommandBuffers();
    void CreateSynchronizationPrimitives();

    void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory);
    void createBufferUnique(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::UniqueBuffer& buffer, vk::UniqueDeviceMemory& bufferMemory);

    void copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);

    void createImage(uint32_t width, uint32_t height, vk::Format format,
                     vk::ImageTiling tiling, vk::ImageUsageFlags usage,
                     vk::MemoryPropertyFlags properties,
                     vk::Image& image, vk::DeviceMemory& imageMemory);

    void createImageUnique(uint32_t width, uint32_t height, vk::Format format,
                     vk::ImageTiling tiling, vk::ImageUsageFlags usage,
                     vk::MemoryPropertyFlags properties,
                     vk::UniqueImage& image, vk::UniqueDeviceMemory& imageMemory);

    vk::CommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(vk::CommandBuffer commandBuffer);

    void transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

    void copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);

    vk::UniqueImageView createImageViewUnique(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags);

    vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
    vk::Format findDepthFormat();
    bool hasStencilComponent(vk::Format format) const;
    QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice _physDevice) const;
    SwapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice _physDevice) const;
    uint32_t VkDemoRenderer::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const;
};

extern VkDemoRenderer VkDemoRender;
