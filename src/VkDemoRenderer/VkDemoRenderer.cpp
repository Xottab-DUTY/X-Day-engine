#include "pch.hpp"

#include <set>
#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "xdCore/Core.hpp"
#include "xdCore/CommandLine.hpp"
#include "xdCore/Filesystem.hpp"
#include "VkDemoEngine.hpp"
#include "VkDemoRenderer.hpp"
#include "Shader.hpp"

namespace std
{
    template <>
    struct hash<VkDemoRenderer::Vertex>
    {
        size_t operator()(VkDemoRenderer::Vertex const& vertex) const
        {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}

namespace XDay::CommandLine
{
Key KeyTexture("texture", "Specifies path to texture file to load, default is \"texture.dds\"", KeyType::String);
Key KeyModel("model", "Specifies path to model file to model, default is \"model.dds\"", KeyType::String);
} // XDay::CommandLine

using namespace XDay;

VkDemoRenderer VkDemoRender;

bool enableValidationLayers = true;

const std::vector<const char*> validationLayers =
{
    "VK_LAYER_LUNARG_standard_validation"
};

const std::vector<const char*> deviceExtensions =
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugReportCallbackEXT(VkInstance instance,
                                        const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
                                        const VkAllocationCallbacks* pAllocator,
                                        VkDebugReportCallbackEXT* pCallback)
{
    const auto func = (PFN_vkCreateDebugReportCallbackEXT)glfwGetInstanceProcAddress(instance, "vkCreateDebugReportCallbackEXT");
    if (func)
        return func(instance, pCreateInfo, pAllocator, pCallback);

    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugReportCallbackEXT(VkInstance instance,
                                                           VkDebugReportCallbackEXT callback,
                                                           const VkAllocationCallbacks* pAllocator)
{
    const auto func = (PFN_vkDestroyDebugReportCallbackEXT)glfwGetInstanceProcAddress(instance, "vkDestroyDebugReportCallbackEXT");
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
    Log::Warning("\nValidation layer reports: \n" \
        "Flags: not working right now \n" \
        "Object type: {} \n" \
        "Object: {} \n" \
        "Location: {} \n" \
        "Code: {} \n" \
        "Layer prefix: {} \n" \
        "Message: {} \n" \
        "User data: {}\n",
        //vk::to_string(flags),
        vk::to_string(objType),
        obj, location,
        code, layerPrefix,
        msg, userData);

    return VK_FALSE;
}

VkDemoRenderer::VkDemoRenderer(): result(vk::Result::eNotReady),
                      swapChainImageFormat(vk::Format::eUndefined) {}

void VkDemoRenderer::Initialize()
{
    enableValidationLayers = Core.isGlobalDebug();
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
    CreateCommandPool();
    CreateDepthResources();
    CreateFramebuffers();
    CreateTextureImage();
    CreateTextureImageView();
    CreateTextureSampler();
    LoadModel();
    CreateVertexBuffer();
    CreateIndexBuffer();
    CreateUniformBuffer();
    CreateDescriptorPool();
    CreateDescriptorSet();
    CreateCommandBuffers();
    CreateSynchronizationPrimitives();
}

void VkDemoRenderer::UpdateUniformBuffer()
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count() / 1000.0f;

    UniformBufferObject ubo;
    ubo.model = glm::rotate(glm::mat4(), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), swapChainExtent.width / static_cast<float>(swapChainExtent.height), 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    void* data = device->mapMemory(*uniformBufferMemory, 0, sizeof(ubo));
    memcpy(data, &ubo, sizeof(ubo));
    device->unmapMemory(*uniformBufferMemory);
}

void VkDemoRenderer::DrawFrame()
{
    uint32_t imageIndex;
    result = device->acquireNextImageKHR(*swapchain, std::numeric_limits<uint64_t>::max(), nullptr, *imageAvailableFence, &imageIndex);

    ASSERT1(result == vk::Result::eSuccess || result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR);

    if (result == vk::Result::eErrorOutOfDateKHR)
    {
        RecreateSwapChain();
        return;
    }

    device->waitForFences(1, &*imageAvailableFence, VK_TRUE, std::numeric_limits<uint64_t>::max());
    device->resetFences(1, &*imageAvailableFence);

    vk::SubmitInfo submitInfo;
    submitInfo.setCommandBufferCount(1);
    submitInfo.setPCommandBuffers(&*commandBuffers[imageIndex]);

    vk::Semaphore signalSemaphores[] = { *renderFinishedSemaphore };
    submitInfo.setSignalSemaphoreCount(1);
    submitInfo.setPSignalSemaphores(signalSemaphores);

    result = graphicsQueue.submit(1, &submitInfo, nullptr);
    ASSERT1(result == vk::Result::eSuccess);

    vk::SwapchainKHR swapChains[] = { *swapchain };

    vk::PresentInfoKHR presentInfo(1, signalSemaphores, 1, swapChains, &imageIndex);

    result = presentQueue.presentKHR(&presentInfo);
    ASSERT1(result == vk::Result::eSuccess || result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR);

    if (result != vk::Result::eSuccess)
        RecreateSwapChain();
}

void VkDemoRenderer::RecreateSwapChain()
{
    device->waitIdle();

    CreateSwapChain();
    CreateImageViews();
    CreateRenderPass();
    CreateGraphicsPipeline();
    CreateDepthResources();
    CreateFramebuffers();
    CreateCommandBuffers();
}

void VkDemoRenderer::InitializeResources()
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

void VkDemoRenderer::CreateVkInstance()
{
    if (enableValidationLayers && !CheckValidationLayersSupport())
        Log::Warning("Vulkan: not all validation layers supported.");

    vk::ApplicationInfo appInfo(Core.GetAppName(), std::stoi(Core.GetAppVersion()),
                                Core.GetEngineName(), std::stoi(Core.GetEngineVersion()),
                                VK_API_VERSION_1_1);

    auto extensions = getRequiredExtensions();

    vk::InstanceCreateInfo i({}, &appInfo, 0, nullptr,
                             static_cast<uint32_t>(extensions.size()),
                             extensions.data());

    if (enableValidationLayers)
    {
        i.setEnabledLayerCount(static_cast<uint32_t>(validationLayers.size()));
        i.setPpEnabledLayerNames(validationLayers.data());
    }

    vkInstance = vk::createInstanceUnique(i);
    ASSERT1(vkInstance);
}

bool VkDemoRenderer::CheckValidationLayersSupport() const
{
    std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();

    for (std::string layerName : validationLayers)
    {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers)
        {
            if (layerName.compare(layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }

        }

        if (!layerFound)
            return false;
    }

    return true;
}

std::vector<const char*> VkDemoRenderer::getRequiredExtensions()
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

void VkDemoRenderer::CreateDebugCallback()
{
    if (!enableValidationLayers) return;

    vk::DebugReportCallbackCreateInfoEXT callbackInfo(
        {vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning},
        (PFN_vkDebugReportCallbackEXT)vkDebugCallback);

    vkCallback = vkInstance->createDebugReportCallbackEXTUnique(callbackInfo);
    ASSERT1(vkCallback);
}

void VkDemoRenderer::CreateVkSurface()
{
    result = (vk::Result)glfwCreateWindowSurface(
        (VkInstance)*vkInstance, VkDemo.windowDemo,
        nullptr, (VkSurfaceKHR*)&*surface);

    ASSERT1(result == vk::Result::eSuccess);
}

void VkDemoRenderer::GetPhysDevice()
{
    std::vector<vk::PhysicalDevice> physDevices = vkInstance->enumeratePhysicalDevices();
    for (const auto& _device : physDevices)
        if (isPhysDeviceSuitable(_device))
        {
            physDevice = _device;
            break;
        }

    ASSERT1(physDevice);
}

bool VkDemoRenderer::isPhysDeviceSuitable(vk::PhysicalDevice _physDevice) const
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

bool VkDemoRenderer::checkDeviceExtensionSupport(vk::PhysicalDevice _physDevice) const
{
    std::vector<vk::ExtensionProperties> availableExtensions = _physDevice.enumerateDeviceExtensionProperties();

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions)
        requiredExtensions.erase(extension.extensionName);

    return requiredExtensions.empty();
}

void VkDemoRenderer::CreateDevice()
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

    device = physDevice.createDeviceUnique(deviceCreateInfo);
    ASSERT1(device);

    device->getQueue(indices.graphicsFamily, 0, &graphicsQueue);
    device->getQueue(indices.presentFamily, 0, &presentQueue);
}

vk::SurfaceFormatKHR VkDemoRenderer::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) const
{
    if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined)
        return { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };

    for (const auto& availableFormat : availableFormats)
        if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            return availableFormat;

    return availableFormats[0];
}

vk::PresentModeKHR VkDemoRenderer::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes) const
{
    for (const auto& availablePresentMode : availablePresentModes)
        if (availablePresentMode == vk::PresentModeKHR::eMailbox)
            return availablePresentMode;

    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D VkDemoRenderer::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        return capabilities.currentExtent;

    vk::Extent2D actualExtent = { static_cast<uint32_t>(VkDemo.currentMode->width), static_cast<uint32_t>(VkDemo.currentMode->height) };

    actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
    actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

    return actualExtent;
}

void VkDemoRenderer::CreateSwapChain()
{
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physDevice);

    vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        imageCount = swapChainSupport.capabilities.maxImageCount;

    vk::SwapchainCreateInfoKHR swapchainInfo = {};
    swapchainInfo.setSurface(*surface);
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

    if (swapchain)
        swapchainInfo.setOldSwapchain(*swapchain);

    swapchain = device->createSwapchainKHRUnique(swapchainInfo);
    ASSERT1(swapchain);

    swapChainImages = device->getSwapchainImagesKHR(*swapchain);
    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}

void VkDemoRenderer::CreateImageViews()
{
    swapChainImageViews.resize(swapChainImages.size());

    for (uint32_t i = 0; i < swapChainImages.size(); i++)
        swapChainImageViews[i] = createImageViewUnique(swapChainImages[i], swapChainImageFormat, vk::ImageAspectFlagBits::eColor);
}

void VkDemoRenderer::CreateRenderPass()
{
    vk::AttachmentDescription colorAttachment;
    colorAttachment.setFormat(swapChainImageFormat);
    colorAttachment.setLoadOp(vk::AttachmentLoadOp::eClear);
    colorAttachment.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
    colorAttachment.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
    colorAttachment.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

    vk::AttachmentDescription depthAttachment;
    depthAttachment.setFormat(findDepthFormat());
    depthAttachment.setSamples(vk::SampleCountFlagBits::e1);
    depthAttachment.setLoadOp(vk::AttachmentLoadOp::eClear);
    depthAttachment.setStoreOp(vk::AttachmentStoreOp::eDontCare);
    depthAttachment.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
    depthAttachment.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
    depthAttachment.setInitialLayout(vk::ImageLayout::eUndefined);
    depthAttachment.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::AttachmentReference colorAttachmentRef(0, vk::ImageLayout::eColorAttachmentOptimal);

    vk::AttachmentReference depthAttachmentRef(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::SubpassDescription subpass;
    subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
    subpass.setColorAttachmentCount(1);
    subpass.setPColorAttachments(&colorAttachmentRef);
    subpass.setPDepthStencilAttachment(&depthAttachmentRef);

    vk::SubpassDependency dependency(VK_SUBPASS_EXTERNAL, 0,
                                                        vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eColorAttachmentOutput,
                                                        {}, vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);

    std::array<vk::AttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };

    vk::RenderPassCreateInfo renderPassInfo;
    renderPassInfo.setAttachmentCount(static_cast<uint32_t>(attachments.size()));
    renderPassInfo.setPAttachments(attachments.data());
    renderPassInfo.setSubpassCount(1);
    renderPassInfo.setPSubpasses(&subpass);
    renderPassInfo.setDependencyCount(1);
    renderPassInfo.setPDependencies(&dependency);

    renderPass = device->createRenderPassUnique(renderPassInfo);
    ASSERT1(renderPass);
}

void VkDemoRenderer::CreateDescriptorSetLayout()
{
    vk::DescriptorSetLayoutBinding uboLayoutBinding(
        0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex);
    
    vk::DescriptorSetLayoutBinding samplerLayoutBinding(
        1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment);

    std::array<vk::DescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
    vk::DescriptorSetLayoutCreateInfo layoutInfo({}, static_cast<uint32_t>(bindings.size()), bindings.data());

    descriptorSetLayout = device->createDescriptorSetLayoutUnique(layoutInfo);
    ASSERT1(descriptorSetLayout);
}

void VkDemoRenderer::CreateGraphicsPipeline()
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

    vk::PipelineDepthStencilStateCreateInfo depthStencil({}, VK_TRUE, VK_TRUE, vk::CompareOp::eLess);

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

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo({}, 1, &*descriptorSetLayout);

    pipelineLayout = device->createPipelineLayoutUnique(pipelineLayoutInfo);
    ASSERT1(pipelineLayout);

    vk::GraphicsPipelineCreateInfo pipelineInfo;
    pipelineInfo.setStageCount(2);
    pipelineInfo.setPStages(shaderStages);
    pipelineInfo.setPVertexInputState(&vertexInputInfo);
    pipelineInfo.setPInputAssemblyState(&inputAssembly);
    pipelineInfo.setPViewportState(&viewportState);
    pipelineInfo.setPRasterizationState(&rasterizer);
    pipelineInfo.setPMultisampleState(&multisampling);
    pipelineInfo.setPDepthStencilState(&depthStencil);
    pipelineInfo.setPColorBlendState(&colorBlending);
    //pipelineInfo.setPDynamicState(&dynamicState); // Optional
    pipelineInfo.setLayout(*pipelineLayout);
    pipelineInfo.setRenderPass(*renderPass);

    graphicsPipeline = device->createGraphicsPipelineUnique(nullptr, pipelineInfo, nullptr);
    ASSERT1(graphicsPipeline);
}

void VkDemoRenderer::CreateFramebuffers()
{
    swapChainFramebuffers.resize(swapChainImageViews.size());

    for (size_t i = 0; i < swapChainImageViews.size(); ++i)
    {
        std::array<vk::ImageView, 2> attachments =
        {
            *swapChainImageViews[i],
            *depthImageView
        };

        vk::FramebufferCreateInfo framebufferInfo({}, *renderPass, static_cast<uint32_t>(attachments.size()),
                                                  attachments.data(), swapChainExtent.width, swapChainExtent.height, 1);

        swapChainFramebuffers[i] = device->createFramebufferUnique(framebufferInfo);
        ASSERT1(swapChainFramebuffers[i]);
    }
}

void VkDemoRenderer::CreateCommandPool()
{
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physDevice);

    vk::CommandPoolCreateInfo poolInfo({}, queueFamilyIndices.graphicsFamily);

    commandPool = device->createCommandPoolUnique(poolInfo);
    ASSERT1(commandPool);
}

void VkDemoRenderer::CreateDepthResources()
{
    vk::Format depthFormat = findDepthFormat();

    createImageUnique(swapChainExtent.width, swapChainExtent.height,
                depthFormat, vk::ImageTiling::eOptimal,
                vk::ImageUsageFlagBits::eDepthStencilAttachment,
                vk::MemoryPropertyFlagBits::eDeviceLocal,
                depthImage, depthImageMemory);

    depthImageView = createImageViewUnique(*depthImage, depthFormat, vk::ImageAspectFlagBits::eDepth);

    transitionImageLayout(*depthImage, depthFormat, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);
}

void VkDemoRenderer::CreateTextureImage()
{
    auto _path = FS.TexturesPath.string() + "texture.dds";

    if (CommandLine::KeyTexture.IsSet())
        _path = FS.TexturesPath.string() + CommandLine::KeyTexture.StringValue();

    gli::texture2d tex2D(gli::load(_path));

    vk::DeviceSize imageSize = tex2D[0].extent().x * tex2D[0].extent().y * 4;

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    createBuffer(imageSize,
                 vk::BufferUsageFlagBits::eTransferSrc,
                 vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                 stagingBuffer, stagingBufferMemory);

    auto data = device->mapMemory(stagingBufferMemory, 0, imageSize);
    memcpy(data, tex2D[0].data(), static_cast<size_t>(imageSize));
    device->unmapMemory(stagingBufferMemory);

    createImageUnique(tex2D[0].extent().x, tex2D[0].extent().y,
                vk::Format::eA8B8G8R8UnormPack32, vk::ImageTiling::eOptimal,
                vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
                vk::MemoryPropertyFlagBits::eDeviceLocal, textureImage, textureImageMemory);

    transitionImageLayout(*textureImage, vk::Format::eA8B8G8R8UnormPack32,
                          vk::ImageLayout::ePreinitialized, vk::ImageLayout::eTransferDstOptimal);

    copyBufferToImage(stagingBuffer, *textureImage, tex2D[0].extent().x, tex2D[0].extent().y);

    transitionImageLayout(*textureImage, vk::Format::eA8B8G8R8UnormPack32,
                          vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

    device->destroyBuffer(stagingBuffer);
    device->freeMemory(stagingBufferMemory);
}

void VkDemoRenderer::CreateTextureImageView()
{
    textureImageView = createImageViewUnique(*textureImage, vk::Format::eA8B8G8R8UnormPack32, vk::ImageAspectFlagBits::eColor);

}

void VkDemoRenderer::CreateTextureSampler()
{
    vk::SamplerCreateInfo samplerInfo({}, vk::Filter::eLinear, vk::Filter::eLinear);
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

    textureSampler = device->createSamplerUnique(samplerInfo);
    ASSERT1(textureSampler);
}

void VkDemoRenderer::LoadModel()
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;

    auto _path = FS.ModelsPath.string() + "model.obj";

    if (CommandLine::KeyModel.IsSet())
        _path = FS.ModelsPath.string() + CommandLine::KeyModel.StringValue();

    Log::Debug("VkDemoRenderer::LoadModel():: loading model: {}", _path);
    if (!LoadObj(&attrib, &shapes, &materials, &err, _path.c_str()))
    {
        Log::Error("VkDemoRenderer::LoadModel():: {}", err);
        throw std::runtime_error(err);
    }

    std::unordered_map<Vertex, uint32_t> uniqueVertices;

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            Vertex vertex = {};

            vertex.pos = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.texCoord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
            };

            vertex.color = {1.0f, 1.0f, 1.0f};

            if (uniqueVertices.count(vertex) == 0)
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }

            indices.push_back(uniqueVertices[vertex]);
        }
    }
    Log::Debug("VkDemoRenderer::LoadModel():: loaded model: {}", _path);
}

void VkDemoRenderer::CreateVertexBuffer()
{
    vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
                 vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                 stagingBuffer, stagingBufferMemory);

    auto data = device->mapMemory(stagingBufferMemory, 0, bufferSize);
    memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
    device->unmapMemory(stagingBufferMemory);

    createBufferUnique(bufferSize,
                 vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
                 vk::MemoryPropertyFlagBits::eDeviceLocal, vertexBuffer, vertexBufferMemory);

    copyBuffer(stagingBuffer, *vertexBuffer, bufferSize);

    device->destroyBuffer(stagingBuffer);
    device->freeMemory(stagingBufferMemory);
}

void VkDemoRenderer::CreateIndexBuffer()
{
    vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    createBuffer(bufferSize,
                 vk::BufferUsageFlagBits::eTransferSrc,
                 vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                 stagingBuffer, stagingBufferMemory);

    auto data = device->mapMemory(stagingBufferMemory, 0, bufferSize);
    memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
    device->unmapMemory(stagingBufferMemory);

    createBufferUnique(bufferSize,
                 vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
                 vk::MemoryPropertyFlagBits::eDeviceLocal, indexBuffer, indexBufferMemory);

    copyBuffer(stagingBuffer, *indexBuffer, bufferSize);

    device->destroyBuffer(stagingBuffer);
    device->freeMemory(stagingBufferMemory);
}

void VkDemoRenderer::CreateUniformBuffer()
{
    vk::DeviceSize bufferSize = sizeof(UniformBufferObject);

    createBufferUnique(bufferSize,
                 vk::BufferUsageFlagBits::eUniformBuffer,
                 vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                 uniformBuffer, uniformBufferMemory);
}

void VkDemoRenderer::CreateDescriptorPool()
{
    std::array<vk::DescriptorPoolSize, 2> poolSizes;
    poolSizes[0].setType(vk::DescriptorType::eUniformBuffer);
    poolSizes[0].setDescriptorCount(1);
    poolSizes[1].setType(vk::DescriptorType::eCombinedImageSampler);
    poolSizes[1].setDescriptorCount(1);

    vk::DescriptorPoolCreateInfo poolInfo({}, 1, static_cast<uint32_t>(poolSizes.size()), poolSizes.data());

    descriptorPool = device->createDescriptorPoolUnique(poolInfo);
    ASSERT1(descriptorPool);
}

void VkDemoRenderer::CreateDescriptorSet()
{
    vk::DescriptorSetLayout layouts[] = { *descriptorSetLayout };
    vk::DescriptorSetAllocateInfo allocInfo(*descriptorPool, 1, layouts);
    result = device->allocateDescriptorSets(&allocInfo, &descriptorSet);
    ASSERT1(result == vk::Result::eSuccess);

    vk::DescriptorBufferInfo bufferInfo(*uniformBuffer, 0, sizeof(UniformBufferObject));

    vk::DescriptorImageInfo imageInfo(*textureSampler, *textureImageView, vk::ImageLayout::eShaderReadOnlyOptimal);

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

    device->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

void VkDemoRenderer::CreateCommandBuffers()
{
    commandBuffers.resize(swapChainFramebuffers.size());

    vk::CommandBufferAllocateInfo allocInfo(
        *commandPool, vk::CommandBufferLevel::ePrimary,
        static_cast<uint32_t>(commandBuffers.size()));

    commandBuffers = device->allocateCommandBuffersUnique(allocInfo);
    ASSERT1(!commandBuffers.empty());

    for (size_t i = 0; i < commandBuffers.size(); ++i)
    {
        vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eSimultaneousUse);

        commandBuffers[i]->begin(&beginInfo);

        vk::RenderPassBeginInfo renderPassInfo;
        renderPassInfo.setRenderPass(*renderPass);
        renderPassInfo.setFramebuffer(*swapChainFramebuffers[i]);
        renderPassInfo.renderArea.setExtent(swapChainExtent);

        std::array<VkClearValue, 2> clearValues = {};
        clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };

        renderPassInfo.setClearValueCount(static_cast<uint32_t>(clearValues.size()));
        renderPassInfo.setPClearValues(reinterpret_cast<vk::ClearValue*>(clearValues.data()));

        commandBuffers[i]->beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);
        commandBuffers[i]->bindPipeline(vk::PipelineBindPoint::eGraphics, *graphicsPipeline);

        vk::Buffer vertexBuffers[] = { *vertexBuffer };
        vk::DeviceSize offsets[] = { 0 };
        commandBuffers[i]->bindVertexBuffers(0, 1, vertexBuffers, offsets);
        commandBuffers[i]->bindIndexBuffer(*indexBuffer, 0, vk::IndexType::eUint32);
        commandBuffers[i]->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

        commandBuffers[i]->drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

        commandBuffers[i]->endRenderPass();
        commandBuffers[i]->end();
    }
}

void VkDemoRenderer::CreateSynchronizationPrimitives()
{
    vk::FenceCreateInfo fenceInfo;
    imageAvailableFence = device->createFenceUnique(fenceInfo);
    ASSERT1(imageAvailableFence);

    vk::SemaphoreCreateInfo semaphoreInfo;
    renderFinishedSemaphore = device->createSemaphoreUnique(semaphoreInfo);
    ASSERT1(renderFinishedSemaphore);
}

void VkDemoRenderer::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory)
{
    vk::BufferCreateInfo bufferInfo({}, size, usage, vk::SharingMode::eExclusive);

    buffer = device->createBuffer(bufferInfo);

    auto memRequirements = device->getBufferMemoryRequirements(buffer);

    vk::MemoryAllocateInfo allocInfo(memRequirements.size, findMemoryType(memRequirements.memoryTypeBits, properties));

    bufferMemory = device->allocateMemory(allocInfo);

    device->bindBufferMemory(buffer, bufferMemory, 0);
}

void VkDemoRenderer::createBufferUnique(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::UniqueBuffer& buffer, vk::UniqueDeviceMemory& bufferMemory)
{
    vk::BufferCreateInfo bufferInfo({}, size, usage, vk::SharingMode::eExclusive);

    buffer = device->createBufferUnique(bufferInfo);

    auto memRequirements = device->getBufferMemoryRequirements(*buffer);

    vk::MemoryAllocateInfo allocInfo(memRequirements.size, findMemoryType(memRequirements.memoryTypeBits, properties));

    bufferMemory = device->allocateMemoryUnique(allocInfo);

    device->bindBufferMemory(*buffer, *bufferMemory, 0);
}

void VkDemoRenderer::copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
{
    vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

    vk::BufferCopy copyRegion(0, 0, size);

    commandBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer);
}

void VkDemoRenderer::createImage(uint32_t width, uint32_t height, vk::Format format,
                                 vk::ImageTiling tiling, vk::ImageUsageFlags usage,
                                 vk::MemoryPropertyFlags properties,
                                 vk::Image& image, vk::DeviceMemory& imageMemory)
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

    image = device->createImage(imageInfo);
    ASSERT1(image);

    auto memRequirements = device->getImageMemoryRequirements(image);
    vk::MemoryAllocateInfo allocInfo(memRequirements.size, findMemoryType(memRequirements.memoryTypeBits, properties));

    imageMemory = device->allocateMemory(allocInfo);
    ASSERT1(imageMemory);

    device->bindImageMemory(image, imageMemory, 0);
}

void VkDemoRenderer::createImageUnique(uint32_t width, uint32_t height, vk::Format format,
                                 vk::ImageTiling tiling, vk::ImageUsageFlags usage,
                                 vk::MemoryPropertyFlags properties,
                                 vk::UniqueImage& image, vk::UniqueDeviceMemory& imageMemory)
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

    image = device->createImageUnique(imageInfo);
    ASSERT1(image);

    auto memRequirements = device->getImageMemoryRequirements(*image);

    vk::MemoryAllocateInfo allocInfo(memRequirements.size, findMemoryType(memRequirements.memoryTypeBits, properties));
    imageMemory = device->allocateMemoryUnique(allocInfo);

    ASSERT1(imageMemory);

    device->bindImageMemory(*image, *imageMemory, 0);
}

vk::CommandBuffer VkDemoRenderer::beginSingleTimeCommands()
{
    vk::CommandBufferAllocateInfo allocInfo(*commandPool, vk::CommandBufferLevel::ePrimary, 1);

    vk::CommandBuffer commandBuffer;
    device->allocateCommandBuffers(&allocInfo, &commandBuffer);

    vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    commandBuffer.begin(beginInfo);

    return commandBuffer;
}

void VkDemoRenderer::endSingleTimeCommands(vk::CommandBuffer commandBuffer)
{
    commandBuffer.end();

    vk::SubmitInfo submitInfo;
    submitInfo.setCommandBufferCount(1);
    submitInfo.setPCommandBuffers(&commandBuffer);

    graphicsQueue.submit(1, &submitInfo, nullptr);
    graphicsQueue.waitIdle();
    device->freeCommandBuffers(*commandPool, 1, &commandBuffer);
}

void VkDemoRenderer::transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
    vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

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
        Log::Error("VkDemoRenderer::transitionImageLayout():: unsupported layout transition!");
        throw std::invalid_argument("unsupported layout transition!");
    }
        

    commandBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTopOfPipe,
        {},
        0, nullptr,
        0, nullptr,
        1, &barrier);

    endSingleTimeCommands(commandBuffer);
}

void VkDemoRenderer::copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height)
{
    vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

    vk::ImageSubresourceLayers subres(vk::ImageAspectFlagBits::eColor, 0, 0, 1);
    vk::BufferImageCopy region(0, 0, 0, subres, {0, 0, 0}, {width, height, 1});

    commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &region);

    endSingleTimeCommands(commandBuffer);
}

vk::UniqueImageView VkDemoRenderer::createImageViewUnique(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags)
{
    vk::ImageSubresourceRange range(aspectFlags, 0, 1, 0, 1);
    vk::ImageViewCreateInfo viewInfo({}, image, vk::ImageViewType::e2D, format);
    viewInfo.setSubresourceRange(range);

    return device->createImageViewUnique(viewInfo);
}

vk::Format VkDemoRenderer::findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
{
    for (auto format : candidates)
    {
        vk::FormatProperties props = physDevice.getFormatProperties(format);

        if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features)
            return format;

        if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features)
            return format;
    }
    Log::Error("VkDemoRenderer::findSupportedFormat():: failed to find supported format!");
    return vk::Format::eUndefined;
}

vk::Format VkDemoRenderer::findDepthFormat()
{
    return findSupportedFormat(
        { vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment
    );
}

bool VkDemoRenderer::hasStencilComponent(vk::Format format) const
{
    return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}

bool VkDemoRenderer::QueueFamilyIndices::isComplete() const
{
    return graphicsFamily >= 0 && presentFamily >= 0;
}

VkDemoRenderer::QueueFamilyIndices VkDemoRenderer::findQueueFamilies(vk::PhysicalDevice _physDevice) const
{
    QueueFamilyIndices indices;

    std::vector<vk::QueueFamilyProperties> queueFamilies = _physDevice.getQueueFamilyProperties();

    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
            indices.graphicsFamily = i;

        vk::Bool32 presentSupport = false;
        _physDevice.getSurfaceSupportKHR(i, *surface, &presentSupport);

        if (queueFamily.queueCount > 0 && presentSupport)
            indices.presentFamily = i;

        if (indices.isComplete())
            break;

        ++i;
    }

    return indices;
}

VkDemoRenderer::SwapChainSupportDetails VkDemoRenderer::querySwapChainSupport(vk::PhysicalDevice _physDevice) const
{
    SwapChainSupportDetails details;

    _physDevice.getSurfaceCapabilitiesKHR(*surface, &details.capabilities);

    details.formats = _physDevice.getSurfaceFormatsKHR(*surface);
    details.presentModes = _physDevice.getSurfacePresentModesKHR(*surface);

    return details;
}

vk::VertexInputBindingDescription VkDemoRenderer::Vertex::getBindingDescription()
{
    vk::VertexInputBindingDescription bindingDescription(0, sizeof(Vertex), vk::VertexInputRate::eVertex);
    return bindingDescription;
}

std::array<vk::VertexInputAttributeDescription, 3> VkDemoRenderer::Vertex::getAttributeDescriptions()
{
    std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions;
    attributeDescriptions[0].setBinding(0);
    attributeDescriptions[0].setLocation(0);
    attributeDescriptions[0].setFormat(vk::Format::eR32G32B32Sfloat);
    attributeDescriptions[0].setOffset(static_cast<uint32_t>(offsetof(Vertex, pos)));

    attributeDescriptions[1].setBinding(0);
    attributeDescriptions[1].setLocation(1);
    attributeDescriptions[1].setFormat(vk::Format::eR32G32B32Sfloat);
    attributeDescriptions[1].setOffset(static_cast<uint32_t>(offsetof(Vertex, color)));

    attributeDescriptions[2].setBinding(0);
    attributeDescriptions[2].setLocation(2);
    attributeDescriptions[2].setFormat(vk::Format::eR32G32B32Sfloat);
    attributeDescriptions[2].setOffset(static_cast<uint32_t>(offsetof(Vertex, texCoord)));

    return attributeDescriptions;
}

bool VkDemoRenderer::Vertex::operator==(const Vertex& other) const
{
    return pos == other.pos && color == other.color && texCoord == other.texCoord;
}

uint32_t VkDemoRenderer::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const
{
    vk::PhysicalDeviceMemoryProperties memProperties = physDevice.getMemoryProperties();

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;

    Log::Error("VkDemoRenderer::findMemoryType():: failed to find suitable memory type!");
    throw std::runtime_error("failed to find suitable memory type!");
}
