#include <set>

#include "Common/Platform.hpp" // this must be first
#include <vulkan/vulkan.hpp>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <gli/gli.hpp>

#include <GLFW/glfw3.h>
#include <fmt/ostream.h>

#include "xdEngine/Debug/Log.hpp"
#include "xdEngine/xdCore.hpp"
#include "xdEngine/xdEngine.hpp"
#include "Renderer.hpp"
#include "Shader.hpp"

XDAY_API Renderer Render;

const bool enableValidationLayers = Core.isGlobalDebug();

const std::vector<const char*> validationLayers =
{
    "VK_LAYER_LUNARG_standard_validation"
};

const std::vector<const char*> deviceExtensions =
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const std::vector<Renderer::Vertex> vertices =
{
    {{-0.5f, -0.5f},{1.0f, 0.0f, 0.0f},{0.0f, 0.0f}},
    {{0.5f, -0.5f},{0.0f, 1.0f, 0.0f},{1.0f, 0.0f}},
    {{0.5f, 0.5f},{0.0f, 0.0f, 1.0f},{1.0f, 1.0f}},
    {{-0.5f, 0.5f},{1.0f, 1.0f, 1.0f},{0.0f, 1.0f}}
};

const std::vector<uint16_t> indices =
{
    0, 1, 2, 2, 3, 0
};

VkResult vkCreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback)
{
    auto func = (PFN_vkCreateDebugReportCallbackEXT)glfwGetInstanceProcAddress(instance, "vkCreateDebugReportCallbackEXT");
    if (func)
        return func(instance, pCreateInfo, pAllocator, pCallback);

    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void vkDestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator = nullptr)
{
    auto func = (PFN_vkDestroyDebugReportCallbackEXT)glfwGetInstanceProcAddress(instance, "vkDestroyDebugReportCallbackEXT");
    if (func && callback)
        func(instance, callback, pAllocator);
}

static VKAPI_ATTR vk::Bool32 VKAPI_CALL vkDebugCallback(
    vk::DebugReportFlagsEXT flags,
    vk::DebugReportObjectTypeEXT objType,
    uint64_t obj,
    size_t location,
    int32_t code,
    const char* layerPrefix,
    const char* msg,
    void* userData)
{
    Msg("\nValidation layer reports: \n" \
        "Flags: not working right now \n" \
        "Object type: {} \n" \
        "Object: {} \n" \
        "Location: {} \n" \
        "Code: {} \n" \
        "Layer prefix: {} \n" \
        "Message: {} \n" \
        "User data: {}\n",
        vk::to_string(objType),
        obj, location,
        code, layerPrefix,
        msg, userData);

    return VK_FALSE;
}

Renderer::Renderer(): result(vk::Result::eNotReady),
    swapChainImageFormat(vk::Format::eUndefined) {}

void Renderer::Initialize()
{
    glfwSetWindowUserPointer(Engine.window, this);
    glslang::InitializeProcess();

    InitializeResources();
    CreateVkInstance();
    CreateDebugCallback();
    CreateVkSurface();
    GetPhysDevice();
    CreateDevice();
    CreateSwapChain();
    CreateImageViews();
    CreateRenderPass();
    CreateDescriptorSetLayout();
    CreateGraphicsPipeline();
    CreateFramebuffers();
    CreateCommandPool();
    CreateTextureImage();
    CreateTextureImageView();
    CreateTextureSampler();
    CreateVertexBuffer();
    CreateIndexBuffer();
    CreateUniformBuffer();
    CreateDescriptorPool();
    CreateDescriptorSet();
    CreateCommandBuffers();
    CreateSynchronizationPrimitives();
}

void Renderer::Destroy()
{
    glslang::FinalizeProcess();

    device.destroySampler(textureSampler);
    device.destroyImageView(textureImageView);

    device.destroyImage(textureImage);
    device.freeMemory(textureImageMemory);

    device.destroyDescriptorPool(descriptorPool);
    device.destroyDescriptorSetLayout(descriptorSetLayout);

    device.destroyBuffer(uniformBuffer);
    device.freeMemory(uniformBufferMemory);

    device.destroyBuffer(indexBuffer);
    device.freeMemory(indexBufferMemory);

    device.destroyBuffer(vertexBuffer);
    device.freeMemory(vertexBufferMemory);

    device.destroySemaphore(renderFinishedSemaphore);
    device.destroyFence(imageAvailableFence);
    device.destroyCommandPool(commandPool);

    for (size_t i = 0; i < swapChainFramebuffers.size(); ++i)
        device.destroyFramebuffer(swapChainFramebuffers[i]);

    device.destroyPipeline(graphicsPipeline);
    device.destroyPipelineLayout(pipelineLayout);
    device.destroyRenderPass(renderPass);

    for (size_t i = 0; i < swapChainImageViews.size(); ++i)
        device.destroyImageView(swapChainImageViews[i], nullptr);

    device.destroySwapchainKHR(swapchain);
    device.destroy();
    instance.destroySurfaceKHR(surface);
    instance.destroyDebugReportCallbackEXT(vkCallback);
    instance.destroy();
}

void Renderer::UpdateUniformBuffer()
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count() / 1000.0f;

    UniformBufferObject ubo;
    ubo.model = glm::rotate(glm::mat4(), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), swapChainExtent.width / static_cast<float>(swapChainExtent.height), 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    void* data = device.mapMemory(uniformBufferMemory, 0, sizeof(ubo));
    memcpy(data, &ubo, sizeof(ubo));
    device.unmapMemory(uniformBufferMemory);
}

void Renderer::DrawFrame()
{
    uint32_t imageIndex;
    result = device.acquireNextImageKHR(swapchain, std::numeric_limits<uint64_t>::max(), nullptr, imageAvailableFence, &imageIndex);

    assert(result == vk::Result::eSuccess || result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR);

    if (result == vk::Result::eErrorOutOfDateKHR)
    {
        RecreateSwapChain();
        return;
    }

    device.waitForFences(1, &imageAvailableFence, VK_TRUE, std::numeric_limits<uint64_t>::max());
    device.resetFences(1, &imageAvailableFence);

    vk::SubmitInfo submitInfo;
    submitInfo.setCommandBufferCount(1);
    submitInfo.setPCommandBuffers(&commandBuffers[imageIndex]);

    vk::Semaphore signalSemaphores[] = { renderFinishedSemaphore };
    submitInfo.setSignalSemaphoreCount(1);
    submitInfo.setPSignalSemaphores(signalSemaphores);

    result = graphicsQueue.submit(1, &submitInfo, nullptr);
    assert(result == vk::Result::eSuccess);

    vk::SwapchainKHR swapChains[] = { swapchain };

    vk::PresentInfoKHR presentInfo(1, signalSemaphores, 1, swapChains, &imageIndex);

    result = presentQueue.presentKHR(&presentInfo);
    assert(result == vk::Result::eSuccess || result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR);

    if (result != vk::Result::eSuccess)
        RecreateSwapChain();
}

void Renderer::RecreateSwapChain()
{
    device.waitIdle();

    CleanupSwapChain();

    CreateSwapChain();
    CreateImageViews();
    CreateRenderPass();
    CreateGraphicsPipeline();
    CreateFramebuffers();
    CreateCommandBuffers();
}

void Renderer::CleanupSwapChain()
{
    for (size_t i = 0; i < swapChainFramebuffers.size(); ++i)
        device.destroyFramebuffer(swapChainFramebuffers[i]);

    device.destroyPipeline(graphicsPipeline);
    device.destroyPipelineLayout(pipelineLayout);
    device.destroyRenderPass(renderPass);

    for (size_t i = 0; i < swapChainImageViews.size(); ++i)
        device.destroyImageView(swapChainImageViews[i], nullptr);
}

void Renderer::InitializeResources()
{
    resources.maxLights = 32;
    resources.maxClipPlanes = 6;
    resources.maxTextureUnits = 32;
    resources.maxTextureCoords = 32;
    resources.maxVertexAttribs = 64;
    resources.maxVertexUniformComponents = 4096;
    resources.maxVaryingFloats = 64;
    resources.maxVertexTextureImageUnits = 32;
    resources.maxCombinedTextureImageUnits = 80;
    resources.maxTextureImageUnits = 32;
    resources.maxFragmentUniformComponents = 4096;
    resources.maxDrawBuffers = 32;
    resources.maxVertexUniformVectors = 128;
    resources.maxVaryingVectors = 8;
    resources.maxFragmentUniformVectors = 16;
    resources.maxVertexOutputVectors = 16;
    resources.maxFragmentInputVectors = 15;
    resources.minProgramTexelOffset = -8;
    resources.maxProgramTexelOffset = 7;
    resources.maxClipDistances = 8;
    resources.maxComputeWorkGroupCountX = 65535;
    resources.maxComputeWorkGroupCountY = 65535;
    resources.maxComputeWorkGroupCountZ = 65535;
    resources.maxComputeWorkGroupSizeX = 1024;
    resources.maxComputeWorkGroupSizeY = 1024;
    resources.maxComputeWorkGroupSizeZ = 64;
    resources.maxComputeUniformComponents = 1024;
    resources.maxComputeTextureImageUnits = 16;
    resources.maxComputeImageUniforms = 8;
    resources.maxComputeAtomicCounters = 8;
    resources.maxComputeAtomicCounterBuffers = 1;
    resources.maxVaryingComponents = 60;
    resources.maxVertexOutputComponents = 64;
    resources.maxGeometryInputComponents = 64;
    resources.maxGeometryOutputComponents = 128;
    resources.maxFragmentInputComponents = 128;
    resources.maxImageUnits = 8;
    resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
    resources.maxCombinedShaderOutputResources = 8;
    resources.maxImageSamples = 0;
    resources.maxVertexImageUniforms = 0;
    resources.maxTessControlImageUniforms = 0;
    resources.maxTessEvaluationImageUniforms = 0;
    resources.maxGeometryImageUniforms = 0;
    resources.maxFragmentImageUniforms = 8;
    resources.maxCombinedImageUniforms = 8;
    resources.maxGeometryTextureImageUnits = 16;
    resources.maxGeometryOutputVertices = 256;
    resources.maxGeometryTotalOutputComponents = 1024;
    resources.maxGeometryUniformComponents = 1024;
    resources.maxGeometryVaryingComponents = 64;
    resources.maxTessControlInputComponents = 128;
    resources.maxTessControlOutputComponents = 128;
    resources.maxTessControlTextureImageUnits = 16;
    resources.maxTessControlUniformComponents = 1024;
    resources.maxTessControlTotalOutputComponents = 4096;
    resources.maxTessEvaluationInputComponents = 128;
    resources.maxTessEvaluationOutputComponents = 128;
    resources.maxTessEvaluationTextureImageUnits = 16;
    resources.maxTessEvaluationUniformComponents = 1024;
    resources.maxTessPatchComponents = 120;
    resources.maxPatchVertices = 32;
    resources.maxTessGenLevel = 64;
    resources.maxViewports = 16;
    resources.maxVertexAtomicCounters = 0;
    resources.maxTessControlAtomicCounters = 0;
    resources.maxTessEvaluationAtomicCounters = 0;
    resources.maxGeometryAtomicCounters = 0;
    resources.maxFragmentAtomicCounters = 8;
    resources.maxCombinedAtomicCounters = 8;
    resources.maxAtomicCounterBindings = 1;
    resources.maxVertexAtomicCounterBuffers = 0;
    resources.maxTessControlAtomicCounterBuffers = 0;
    resources.maxTessEvaluationAtomicCounterBuffers = 0;
    resources.maxGeometryAtomicCounterBuffers = 0;
    resources.maxFragmentAtomicCounterBuffers = 1;
    resources.maxCombinedAtomicCounterBuffers = 1;
    resources.maxAtomicCounterBufferSize = 16384;
    resources.maxTransformFeedbackBuffers = 4;
    resources.maxTransformFeedbackInterleavedComponents = 64;
    resources.maxCullDistances = 8;
    resources.maxCombinedClipAndCullDistances = 8;
    resources.maxSamples = 4;
    resources.limits.nonInductiveForLoops = 1;
    resources.limits.whileLoops = 1;
    resources.limits.doWhileLoops = 1;
    resources.limits.generalUniformIndexing = 1;
    resources.limits.generalAttributeMatrixVectorIndexing = 1;
    resources.limits.generalVaryingIndexing = 1;
    resources.limits.generalSamplerIndexing = 1;
    resources.limits.generalVariableIndexing = 1;
    resources.limits.generalConstantMatrixVectorIndexing = 1;
}

void Renderer::CreateVkInstance()
{
    if (enableValidationLayers && !CheckValidationLayersSupport())
        Log("Vulkan: not all validation layers supported.");

    vk::ApplicationInfo appInfo(Core.AppName.c_str(), stoi(Core.AppVersion),
                                Core.EngineName.c_str(), stoi(Core.EngineVersion), 
                                VK_MAKE_VERSION(1, 0, 42));

    vk::InstanceCreateInfo i;
    i.setPApplicationInfo(&appInfo);

    auto extensions = getRequiredExtensions();    
    i.setEnabledExtensionCount(static_cast<uint32_t>(extensions.size()));
    i.setPpEnabledExtensionNames(extensions.data());

    if (enableValidationLayers)
    {
        i.setEnabledLayerCount(static_cast<uint32_t>(validationLayers.size()));
        i.setPpEnabledLayerNames(validationLayers.data());
    }

    result = vk::createInstance(&i, nullptr, &instance);
    assert(result == vk::Result::eSuccess);
}

bool Renderer::CheckValidationLayersSupport() const
{
    std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();

    for (std::string layerName : validationLayers)
    {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers)
            if (layerName.compare(layerProperties.layerName))
            {
                layerFound = true;
                break;
            }

        if (!layerFound)
            return false;
    }

    return true;
}

std::vector<const char*> Renderer::getRequiredExtensions()
{
    std::vector<const char*> extensions;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    for (uint32_t i = 0; i < glfwExtensionCount; ++i)
        extensions.push_back(glfwExtensions[i]);

    if (enableValidationLayers)
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

    return extensions;
}

void Renderer::CreateDebugCallback()
{
    if (!enableValidationLayers) return;

    vk::DebugReportCallbackCreateInfoEXT callbackInfo
        ({vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning},
        (PFN_vkDebugReportCallbackEXT)vkDebugCallback);

    vkCallback = instance.createDebugReportCallbackEXT(callbackInfo);
    assert(vkCallback);
}

void Renderer::CreateVkSurface()
{
    result = (vk::Result)glfwCreateWindowSurface(
        (VkInstance)instance, Engine.window,
        nullptr, (VkSurfaceKHR*)&surface);

    assert(result == vk::Result::eSuccess);
}

void Renderer::GetPhysDevice()
{
    std::vector<vk::PhysicalDevice> physDevices = instance.enumeratePhysicalDevices();
    for (const auto& _device : physDevices)
        if (isPhysDeviceSuitable(_device))
        {
            physDevice = _device;
            break;
        }

    assert(physDevice);
}

bool Renderer::isPhysDeviceSuitable(vk::PhysicalDevice _physDevice) const
{
    QueueFamilyIndices indices = findQueueFamilies(_physDevice);

    bool extensionsSupported = checkDeviceExtensionSupport(_physDevice);

    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(_physDevice);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    auto supportedFeatures = _physDevice.getFeatures();

    return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

bool Renderer::checkDeviceExtensionSupport(vk::PhysicalDevice _physDevice) const
{
    std::vector<vk::ExtensionProperties> availableExtensions = _physDevice.enumerateDeviceExtensionProperties();

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions)
        requiredExtensions.erase(extension.extensionName);

    return requiredExtensions.empty();
}

void Renderer::CreateDevice()
{
    QueueFamilyIndices indices = findQueueFamilies(physDevice);

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    std::set<int> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };

    float queuePriority = 1.0f;
    for (int queueFamily : uniqueQueueFamilies)
    {
        vk::DeviceQueueCreateInfo queueCreateInfo({}, queueFamily, 1, &queuePriority);
        queueCreateInfos.push_back(queueCreateInfo);
    }
    
    vk::PhysicalDeviceFeatures deviceFeatures;
    deviceFeatures.setSamplerAnisotropy(VK_TRUE);

    vk::DeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers)
    {
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
        deviceCreateInfo.enabledLayerCount = 0;

    device = physDevice.createDevice(deviceCreateInfo);
    assert(device);

    device.getQueue(indices.graphicsFamily, 0, &graphicsQueue);
    device.getQueue(indices.presentFamily, 0, &presentQueue);
}

vk::SurfaceFormatKHR Renderer::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) const
{
    if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined)
        return { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };

    for (const auto& availableFormat : availableFormats)
        if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            return availableFormat;

    return availableFormats[0];
}

vk::PresentModeKHR Renderer::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes) const
{
    for (const auto& availablePresentMode : availablePresentModes)
        if (availablePresentMode == vk::PresentModeKHR::eMailbox)
            return availablePresentMode;

    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D Renderer::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        return capabilities.currentExtent;

    vk::Extent2D actualExtent = { static_cast<uint32_t>(Engine.CurrentMode->width), static_cast<uint32_t>(Engine.CurrentMode->height) };

    actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
    actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

    return actualExtent;
}

void Renderer::CreateSwapChain()
{
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physDevice);

    vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        imageCount = swapChainSupport.capabilities.maxImageCount;

    vk::SwapchainCreateInfoKHR swapchainInfo = {};
    swapchainInfo.setSurface(surface);
    swapchainInfo.setMinImageCount(imageCount);
    swapchainInfo.setImageFormat(surfaceFormat.format);
    swapchainInfo.setImageColorSpace(surfaceFormat.colorSpace);
    swapchainInfo.setImageExtent(extent);
    swapchainInfo.setImageArrayLayers(1);
    swapchainInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

    QueueFamilyIndices indices = findQueueFamilies(physDevice);
    uint32_t queueFamilyIndices[] = { static_cast<uint32_t>(indices.graphicsFamily), static_cast<uint32_t>(indices.presentFamily) };

    if (indices.graphicsFamily != indices.presentFamily)
    {
        swapchainInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
        swapchainInfo.setQueueFamilyIndexCount(2);
        swapchainInfo.setPQueueFamilyIndices(queueFamilyIndices);
    }
    else
    {
        swapchainInfo.setImageSharingMode(vk::SharingMode::eExclusive);
        swapchainInfo.setQueueFamilyIndexCount(0); // Optional
        swapchainInfo.setPQueueFamilyIndices(nullptr); // Optional
    }

    swapchainInfo.setPreTransform(swapChainSupport.capabilities.currentTransform);
    swapchainInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
    swapchainInfo.setPresentMode(presentMode);
    swapchainInfo.setClipped(true);

    vk::SwapchainKHR oldSwapChain = swapchain;
    swapchainInfo.setOldSwapchain(oldSwapChain);

    vk::SwapchainKHR newSwapChain = device.createSwapchainKHR(swapchainInfo);
    swapchain = newSwapChain;
    assert(swapchain);

    swapChainImages = device.getSwapchainImagesKHR(swapchain);
    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}

void Renderer::CreateImageViews()
{
    swapChainImageViews.resize(swapChainImages.size());

    for (size_t i = 0; i < swapChainImages.size(); ++i)
    {
        vk::ImageSubresourceRange range(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
        vk::ImageViewCreateInfo imageViewInfo({}, swapChainImages[i], vk::ImageViewType::e2D,
            swapChainImageFormat, vk::ComponentSwizzle::eIdentity, range);

        result = device.createImageView(&imageViewInfo, nullptr, &swapChainImageViews[i]);
        assert(result == vk::Result::eSuccess);
    }
}

void Renderer::CreateRenderPass()
{
    vk::AttachmentDescription colorAttachment;
    colorAttachment.setFormat(swapChainImageFormat);
    colorAttachment.setLoadOp(vk::AttachmentLoadOp::eClear);
    colorAttachment.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
    colorAttachment.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
    colorAttachment.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

    vk::AttachmentReference colorAttachmentRef(0, vk::ImageLayout::eColorAttachmentOptimal);

    vk::SubpassDescription subpass;
    subpass.setColorAttachmentCount(1);
    subpass.setPColorAttachments(&colorAttachmentRef);

    vk::RenderPassCreateInfo renderPassInfo;
    renderPassInfo.setAttachmentCount(1);
    renderPassInfo.setPAttachments(&colorAttachment);
    renderPassInfo.setSubpassCount(1);
    renderPassInfo.setPSubpasses(&subpass);

    renderPass = device.createRenderPass(renderPassInfo);
    assert(renderPass);
}

void Renderer::CreateDescriptorSetLayout()
{
    vk::DescriptorSetLayoutBinding uboLayoutBinding(
        0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex);
    
    vk::DescriptorSetLayoutBinding samplerLayoutBinding(
        1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment);

    std::array<vk::DescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
    vk::DescriptorSetLayoutCreateInfo layoutInfo({}, static_cast<uint32_t>(bindings.size()), bindings.data());

    descriptorSetLayout = device.createDescriptorSetLayout(layoutInfo);
    assert(descriptorSetLayout);
}

void Renderer::CreateGraphicsPipeline()
{
    ShaderWorker shader_frag("shader.frag", device, resources);
    ShaderWorker shader_vert("shader.vert", device, resources);

    vk::PipelineShaderStageCreateInfo shaderStages[] =
    { shader_frag.GetVkShaderStageInfo(), shader_vert.GetVkShaderStageInfo() };

    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo({}, 1, &bindingDescription,
        static_cast<uint32_t>(attributeDescriptions.size()), attributeDescriptions.data());
    
    vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
    inputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);
    inputAssembly.setPrimitiveRestartEnable(VK_FALSE);

    vk::Viewport viewport;
    viewport.setX(0.0f);
    viewport.setY(0.0f);
    viewport.setWidth(static_cast<float>(swapChainExtent.width));
    viewport.setHeight(static_cast<float>(swapChainExtent.height));
    viewport.setMinDepth(0.0f);
    viewport.setMaxDepth(1.0f);

    vk::Rect2D scissor({}, swapChainExtent);

    vk::PipelineViewportStateCreateInfo viewportState;
    viewportState.setViewportCount(1);
    viewportState.setPViewports(&viewport);
    viewportState.setScissorCount(1);
    viewportState.setPScissors(&scissor);

    vk::PipelineRasterizationStateCreateInfo rasterizer;
    rasterizer.setLineWidth(1.0f);
    rasterizer.setCullMode(vk::CullModeFlagBits::eBack);
    rasterizer.setFrontFace(vk::FrontFace::eCounterClockwise);

    vk::PipelineMultisampleStateCreateInfo multisampling;
    multisampling.setMinSampleShading(1.0f); // Optional

    vk::PipelineColorBlendAttachmentState colorBlendAttachment;
    colorBlendAttachment.setColorWriteMask(vk::ColorComponentFlagBits::eR
        | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB
        | vk::ColorComponentFlagBits::eA);
    colorBlendAttachment.setSrcColorBlendFactor(vk::BlendFactor::eOne);
    colorBlendAttachment.setSrcAlphaBlendFactor(vk::BlendFactor::eOne);

    vk::PipelineColorBlendStateCreateInfo colorBlending;
    colorBlending.setLogicOp(vk::LogicOp::eCopy); // Optional
    colorBlending.setAttachmentCount(1);
    colorBlending.setPAttachments(&colorBlendAttachment);

    vk::DynamicState dynamicStates[] =
    {
        vk::DynamicState::eViewport,
        vk::DynamicState::eLineWidth
    };

    vk::PipelineDynamicStateCreateInfo dynamicState({}, 2, dynamicStates);

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo({}, 1, &descriptorSetLayout);

    pipelineLayout = device.createPipelineLayout(pipelineLayoutInfo);
    assert(pipelineLayout);

    vk::GraphicsPipelineCreateInfo pipelineInfo;
    pipelineInfo.setStageCount(2);
    pipelineInfo.setPStages(shaderStages);
    pipelineInfo.setPVertexInputState(&vertexInputInfo);
    pipelineInfo.setPInputAssemblyState(&inputAssembly);
    pipelineInfo.setPViewportState(&viewportState);
    pipelineInfo.setPRasterizationState(&rasterizer);
    pipelineInfo.setPMultisampleState(&multisampling);
    pipelineInfo.setPColorBlendState(&colorBlending);
    //pipelineInfo.setPDynamicState(&dynamicState); // Optional
    pipelineInfo.setLayout(pipelineLayout);
    pipelineInfo.setRenderPass(renderPass);

    graphicsPipeline = device.createGraphicsPipeline(nullptr, pipelineInfo, nullptr);
    assert(graphicsPipeline);
}

void Renderer::CreateFramebuffers()
{
    swapChainFramebuffers.resize(swapChainImageViews.size());

    for (size_t i = 0; i < swapChainImageViews.size(); ++i)
    {
        vk::ImageView attachments[] =
        {
            swapChainImageViews[i]
        };

        vk::FramebufferCreateInfo framebufferInfo({}, renderPass, 1,
            attachments, swapChainExtent.width, swapChainExtent.height, 1);

        swapChainFramebuffers[i] = device.createFramebuffer(framebufferInfo);
        assert(swapChainFramebuffers[i]);
    }
}

void Renderer::CreateCommandPool()
{
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physDevice);

    vk::CommandPoolCreateInfo poolInfo({}, queueFamilyIndices.graphicsFamily);

    commandPool = device.createCommandPool(poolInfo);
    assert(commandPool);
}

void Renderer::CreateTextureImage()
{
    int texWidth = 130, texHeight = 130;
    gli::texture2d tex2D(gli::load(Core.TexturesPath.string() + "texture.dds"));
    vk::DeviceSize imageSize = texWidth * texHeight * 4;

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    createBuffer(imageSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        stagingBuffer, stagingBufferMemory);

    auto data = device.mapMemory(stagingBufferMemory, 0, imageSize);
    memcpy(data, tex2D.data(), static_cast<size_t>(imageSize));
    device.unmapMemory(stagingBufferMemory);

    createImage(texWidth, texHeight,
        vk::Format::eA8B8G8R8UnormPack32, vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
        vk::MemoryPropertyFlagBits::eDeviceLocal, textureImage, textureImageMemory);

    transitionImageLayout(textureImage, vk::Format::eA8B8G8R8UnormPack32,
        vk::ImageLayout::ePreinitialized, vk::ImageLayout::eTransferDstOptimal);

    copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

    transitionImageLayout(textureImage, vk::Format::eA8B8G8R8UnormPack32,
        vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

    device.destroyBuffer(stagingBuffer);
    device.freeMemory(stagingBufferMemory);
}

void Renderer::CreateTextureImageView()
{
    textureImageView = createImageView(textureImage, vk::Format::eA8B8G8R8UnormPack32);
}

void Renderer::CreateTextureSampler()
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

void Renderer::CreateVertexBuffer()
{
    vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        stagingBuffer, stagingBufferMemory);

    auto data = device.mapMemory(stagingBufferMemory, 0, bufferSize);
    memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
    device.unmapMemory(stagingBufferMemory);

    createBuffer(bufferSize,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal, vertexBuffer, vertexBufferMemory);

    copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

    device.destroyBuffer(stagingBuffer);
    device.freeMemory(stagingBufferMemory);
}

void Renderer::CreateIndexBuffer()
{
    vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    createBuffer(bufferSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        stagingBuffer, stagingBufferMemory);

    auto data = device.mapMemory(stagingBufferMemory, 0, bufferSize);
    memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
    device.unmapMemory(stagingBufferMemory);

    createBuffer(bufferSize,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal, indexBuffer, indexBufferMemory);

    copyBuffer(stagingBuffer, indexBuffer, bufferSize);

    device.destroyBuffer(stagingBuffer);
    device.freeMemory(stagingBufferMemory);
}

void Renderer::CreateUniformBuffer()
{
    vk::DeviceSize bufferSize = sizeof(UniformBufferObject);

    createBuffer(bufferSize,
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        uniformBuffer, uniformBufferMemory);
}

void Renderer::CreateDescriptorPool()
{
    std::array<vk::DescriptorPoolSize, 2> poolSizes;
    poolSizes[0].setType(vk::DescriptorType::eUniformBuffer);
    poolSizes[0].setDescriptorCount(1);
    poolSizes[1].setType(vk::DescriptorType::eCombinedImageSampler);
    poolSizes[1].setDescriptorCount(1);

    vk::DescriptorPoolCreateInfo poolInfo({}, 1, static_cast<uint32_t>(poolSizes.size()), poolSizes.data());

    descriptorPool = device.createDescriptorPool(poolInfo);
    assert(descriptorPool);
}

void Renderer::CreateDescriptorSet()
{
    vk::DescriptorSetLayout layouts[] = { descriptorSetLayout };
    vk::DescriptorSetAllocateInfo allocInfo(descriptorPool, 1, layouts);
    result = device.allocateDescriptorSets(&allocInfo, &descriptorSet);
    assert(result == vk::Result::eSuccess);

    vk::DescriptorBufferInfo bufferInfo(uniformBuffer, 0, sizeof(UniformBufferObject));

    vk::DescriptorImageInfo imageInfo(textureSampler, textureImageView, vk::ImageLayout::eShaderReadOnlyOptimal);

    std::array<vk::WriteDescriptorSet, 2> descriptorWrites;

    descriptorWrites[0].setDstSet(descriptorSet);
    descriptorWrites[0].setDstBinding(0);
    descriptorWrites[0].setDstArrayElement(0);
    descriptorWrites[0].setDescriptorType(vk::DescriptorType::eUniformBuffer);
    descriptorWrites[0].setDescriptorCount(1);
    descriptorWrites[0].setPBufferInfo(&bufferInfo);

    descriptorWrites[1].setDstSet(descriptorSet);
    descriptorWrites[1].setDstBinding(1);
    descriptorWrites[1].setDstArrayElement(0);
    descriptorWrites[1].setDescriptorType(vk::DescriptorType::eCombinedImageSampler);
    descriptorWrites[1].setDescriptorCount(1);
    descriptorWrites[1].setPImageInfo(&imageInfo);

    device.updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

void Renderer::CreateCommandBuffers()
{
    if (!commandBuffers.empty())
        device.freeCommandBuffers(commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

    commandBuffers.resize(swapChainFramebuffers.size());

    vk::CommandBufferAllocateInfo allocInfo(
        commandPool, vk::CommandBufferLevel::ePrimary,
        static_cast<uint32_t>(commandBuffers.size()));

    commandBuffers = device.allocateCommandBuffers(allocInfo);
    assert(!commandBuffers.empty());

    for (size_t i = 0; i < commandBuffers.size(); ++i)
    {
        vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eSimultaneousUse);

        commandBuffers[i].begin(&beginInfo);

        vk::RenderPassBeginInfo renderPassInfo;
        renderPassInfo.setRenderPass(renderPass);
        renderPassInfo.setFramebuffer(swapChainFramebuffers[i]);
        renderPassInfo.renderArea.setExtent(swapChainExtent);

        VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
        renderPassInfo.setClearValueCount(1);
        renderPassInfo.setPClearValues(reinterpret_cast<vk::ClearValue*>(&clearColor));

        commandBuffers[i].beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);
        commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);

        vk::Buffer vertexBuffers[] = { vertexBuffer };
        vk::DeviceSize offsets[] = { 0 };
        commandBuffers[i].bindVertexBuffers(0, 1, vertexBuffers, offsets);
        commandBuffers[i].bindIndexBuffer(indexBuffer, 0, vk::IndexType::eUint16);
        commandBuffers[i].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

        commandBuffers[i].drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

        commandBuffers[i].endRenderPass();
        commandBuffers[i].end();
    }
}

void Renderer::CreateSynchronizationPrimitives()
{
    vk::FenceCreateInfo fenceInfo;
    imageAvailableFence = device.createFence(fenceInfo);
    assert(imageAvailableFence);

    vk::SemaphoreCreateInfo semaphoreInfo;
    renderFinishedSemaphore = device.createSemaphore(semaphoreInfo);
    assert(renderFinishedSemaphore);
}

void Renderer::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory)
{
    vk::BufferCreateInfo bufferInfo({}, size, usage, vk::SharingMode::eExclusive);

    buffer = device.createBuffer(bufferInfo);

    auto memRequirements = device.getBufferMemoryRequirements(buffer);

    vk::MemoryAllocateInfo allocInfo(memRequirements.size, findMemoryType(memRequirements.memoryTypeBits, properties));

    bufferMemory = device.allocateMemory(allocInfo);

    device.bindBufferMemory(buffer, bufferMemory, 0);
}

void Renderer::copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
{
    vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

    vk::BufferCopy copyRegion(0, 0, size);

    commandBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer);
}

void Renderer::createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling,
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

vk::CommandBuffer Renderer::beginSingleTimeCommands()
{
    vk::CommandBufferAllocateInfo allocInfo(commandPool, vk::CommandBufferLevel::ePrimary, 1);

    vk::CommandBuffer commandBuffer;
    device.allocateCommandBuffers(&allocInfo, &commandBuffer);

    vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    commandBuffer.begin(beginInfo);

    return commandBuffer;
}

void Renderer::endSingleTimeCommands(vk::CommandBuffer commandBuffer)
{
    commandBuffer.end();

    vk::SubmitInfo submitInfo;
    submitInfo.setCommandBufferCount(1);
    submitInfo.setPCommandBuffers(&commandBuffer);

    graphicsQueue.submit(1, &submitInfo, nullptr);
    graphicsQueue.waitIdle();
    device.freeCommandBuffers(commandPool, 1, &commandBuffer);
}

void Renderer::transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
    vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

    vk::ImageSubresourceRange range(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
    vk::ImageMemoryBarrier barrier({}, {}, oldLayout, newLayout, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, image, range);

    if (oldLayout == vk::ImageLayout::ePreinitialized && newLayout == vk::ImageLayout::eTransferDstOptimal)
    {
        barrier.setSrcAccessMask(vk::AccessFlagBits::eHostWrite);
        barrier.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);
    }
    else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
    {
        barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
        barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);
    }
    else
        throw std::invalid_argument("unsupported layout transition!");

    commandBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTopOfPipe,
        {},
        0, nullptr,
        0, nullptr,
        1, &barrier);

    endSingleTimeCommands(commandBuffer);
}

void Renderer::copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height)
{
    vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

    vk::ImageSubresourceLayers subres(vk::ImageAspectFlagBits::eColor, 0, 0, 1);
    vk::BufferImageCopy region(0, 0, 0, subres, {0, 0, 0}, {width, height, 1});

    commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &region);

    endSingleTimeCommands(commandBuffer);
}

vk::ImageView Renderer::createImageView(vk::Image image, vk::Format format)
{
    vk::ImageViewCreateInfo viewInfo({}, image, vk::ImageViewType::e2D, format);
    viewInfo.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
    viewInfo.subresourceRange.setLevelCount(1);
    viewInfo.subresourceRange.setLayerCount(1);

    auto imageView = device.createImageView(viewInfo);

    return imageView;
}

bool Renderer::QueueFamilyIndices::isComplete() const
{
    return graphicsFamily >= 0 && presentFamily >= 0;
}

Renderer::QueueFamilyIndices Renderer::findQueueFamilies(vk::PhysicalDevice _physDevice) const
{
    QueueFamilyIndices indices;

    std::vector<vk::QueueFamilyProperties> queueFamilies = _physDevice.getQueueFamilyProperties();

    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
            indices.graphicsFamily = i;

        vk::Bool32 presentSupport = false;
        _physDevice.getSurfaceSupportKHR(i, surface, &presentSupport);

        if (queueFamily.queueCount > 0 && presentSupport)
            indices.presentFamily = i;

        if (indices.isComplete())
            break;

        ++i;
    }

    return indices;
}



Renderer::SwapChainSupportDetails Renderer::querySwapChainSupport(vk::PhysicalDevice _physDevice) const
{
    SwapChainSupportDetails details;

    _physDevice.getSurfaceCapabilitiesKHR(surface, &details.capabilities);

    {
        uint32_t formatCount;
        _physDevice.getSurfaceFormatsKHR(surface, &formatCount, nullptr);

        if (formatCount != 0)
        {
            details.formats.resize(static_cast<size_t>(formatCount));
            _physDevice.getSurfaceFormatsKHR(surface, &formatCount, details.formats.data());
        }
    }

    {
        uint32_t presentModeCount;
        _physDevice.getSurfacePresentModesKHR(surface, &presentModeCount, nullptr);

        if (presentModeCount != 0)
        {
            details.presentModes.resize(static_cast<size_t>(presentModeCount));
            _physDevice.getSurfacePresentModesKHR(surface, &presentModeCount, details.presentModes.data());
        }
    }

    return details;
}


vk::VertexInputBindingDescription Renderer::Vertex::getBindingDescription()
{
    vk::VertexInputBindingDescription bindingDescription(0, sizeof(Vertex), vk::VertexInputRate::eVertex);
    return bindingDescription;
}

std::array<vk::VertexInputAttributeDescription, 3> Renderer::Vertex::getAttributeDescriptions()
{
    std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions;
    attributeDescriptions[0].setBinding(0);
    attributeDescriptions[0].setLocation(0);
    attributeDescriptions[0].setFormat(vk::Format::eR32G32Sfloat);
    attributeDescriptions[0].setOffset(static_cast<uint32_t>(offsetof(Vertex, pos)));

    attributeDescriptions[1].setBinding(0);
    attributeDescriptions[1].setLocation(1);
    attributeDescriptions[1].setFormat(vk::Format::eR32G32B32Sfloat);
    attributeDescriptions[1].setOffset(static_cast<uint32_t>(offsetof(Vertex, color)));

    attributeDescriptions[2].setBinding(0);
    attributeDescriptions[2].setLocation(2);
    attributeDescriptions[2].setFormat(vk::Format::eR32G32Sfloat);
    attributeDescriptions[2].setOffset(offsetof(Vertex, texCoord));

    return attributeDescriptions;
}

uint32_t Renderer::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const
{
    vk::PhysicalDeviceMemoryProperties memProperties = physDevice.getMemoryProperties();

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;

    Log("Renderer::findMemoryType():: failed to find suitable memory type!");
    throw std::runtime_error("failed to find suitable memory type!");
}
