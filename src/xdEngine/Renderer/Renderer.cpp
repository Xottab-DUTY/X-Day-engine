#include "Common/Platform.hpp" // this must be first

#include <set>
#include <vulkan/vulkan.hpp>

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
    CreateGraphicsPipeline();
    CreateFramebuffers();
    CreateCommandPool();
    CreateCommandBuffers();
}

void Renderer::Destroy()
{
    glslang::FinalizeProcess();

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

    result = instance.createDebugReportCallbackEXT(&callbackInfo, nullptr, &vkCallback);
    assert(result == vk::Result::eSuccess);
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

struct Renderer::QueueFamilyIndices
{
    int graphicsFamily = -1;
    int presentFamily = -1;

    bool isComplete() const
    {
        return graphicsFamily >= 0 && presentFamily >= 0;
    }
};

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

struct Renderer::SwapChainSupportDetails
{
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

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

    return indices.isComplete() && extensionsSupported && swapChainAdequate;
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
    
    vk::PhysicalDeviceFeatures deviceFeatures = {};

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

    // Another variant
    /*result = physDevice.createDevice(&deviceCreateInfo, nullptr, &device);
    assert(result == vk::Result::eSuccess);*/

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
    swapchainInfo.setOldSwapchain(nullptr);

    result = device.createSwapchainKHR(&swapchainInfo, nullptr, &swapchain);
    assert(result == vk::Result::eSuccess);

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

    result = device.createRenderPass(&renderPassInfo, nullptr, &renderPass);
    assert(result == vk::Result::eSuccess);
}

void Renderer::CreateGraphicsPipeline()
{
    ShaderWorker shader_frag("shader.frag", device, resources);
    ShaderWorker shader_vert("shader.vert", device, resources);

    vk::PipelineShaderStageCreateInfo shaderStages[] =
    { shader_frag.GetVkShaderStageInfo(), shader_vert.GetVkShaderStageInfo() };

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo;

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
    rasterizer.setFrontFace(vk::FrontFace::eClockwise);

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

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo;

    result = device.createPipelineLayout(&pipelineLayoutInfo, nullptr, &pipelineLayout);
    assert(result == vk::Result::eSuccess);

    vk::GraphicsPipelineCreateInfo pipelineInfo;
    pipelineInfo.setStageCount(2);
    pipelineInfo.setPStages(shaderStages);
    pipelineInfo.setPVertexInputState(&vertexInputInfo);
    pipelineInfo.setPInputAssemblyState(&inputAssembly);
    pipelineInfo.setPViewportState(&viewportState);
    pipelineInfo.setPRasterizationState(&rasterizer);
    pipelineInfo.setPMultisampleState(&multisampling);
    pipelineInfo.setPColorBlendState(&colorBlending);
    pipelineInfo.setLayout(pipelineLayout);
    pipelineInfo.setRenderPass(renderPass);

    graphicsPipeline = device.createGraphicsPipeline(nullptr, pipelineInfo, nullptr);
    assert(graphicsPipeline);

    // Another variant
    /*result = device.createGraphicsPipelines(nullptr, 1, &pipelineInfo, nullptr, &graphicsPipeline);
    assert(result == vk::Result::eSuccess);*/
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

        // Another variant
        /*result = device.createFramebuffer(&framebufferInfo, nullptr, &swapChainFramebuffers[i]);
        assert(result == vk::Result::eSuccess);*/
    }
}

void Renderer::CreateCommandPool()
{
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physDevice);

    vk::CommandPoolCreateInfo poolInfo({}, queueFamilyIndices.graphicsFamily);

    commandPool = device.createCommandPool(poolInfo);
    assert(commandPool);

    // Another variant
    /*result = device.createCommandPool(&poolInfo, nullptr, &commandPool);
    assert(result == vk::Result::eSuccess);*/
}

void Renderer::CreateCommandBuffers()
{
    commandBuffers.resize(swapChainFramebuffers.size());

    vk::CommandBufferAllocateInfo allocInfo(
        commandPool, vk::CommandBufferLevel::ePrimary,
        static_cast<uint32_t>(commandBuffers.size()));

    commandBuffers = device.allocateCommandBuffers(allocInfo);
    assert(!commandBuffers.empty());

    // Another variant
    /*result = device.allocateCommandBuffers(&allocInfo, commandBuffers.data());
    assert(result == vk::Result::eSuccess);*/

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
        commandBuffers[i].draw(3,1,0,0);
        commandBuffers[i].endRenderPass();
        commandBuffers[i].end();
    }
}
