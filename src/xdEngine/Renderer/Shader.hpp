#pragma once
#ifndef Shader_hpp__
#define Shader_hpp__
#include "Common/Platform.hpp" // this must be first
#include <vulkan/vulkan.hpp>

#include <ShaderLang.h>

class ShaderWorker // TODO: wip name; rename it, maybe.
{
    std::string shaderName;
    std::string binaryExt;
    const vk::Device& device;
    const TBuiltInResource& resources;
    vk::ShaderModule shaderModule;
    vk::PipelineShaderStageCreateInfo stageInfo;
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
    vk::Result result;

    std::vector<char> shaderSource;
    std::vector<char> binaryShader;

    bool sourceFound;
    bool binaryFound;
    bool binaryIsOld;

public:
    ShaderWorker(std::string _name, const vk::Device& _device, const TBuiltInResource& _resources);
    ~ShaderWorker();
    void Initialize();

    bool isSourceFound() const;
    bool isBinaryFound() const;
    bool isBinaryOld() const;

    vk::ShaderModule GetShaderModule() const;
    vk::PipelineShaderStageCreateInfo GetVkShaderStageInfo() const;
private:
    void LoadShader();
    void LoadBinaryShader();
    bool CheckIfShaderChanged();
    void CompileShader();
    void CreateVkShaderModule();
    void SetVkShaderStage();

    EShLanguage GetLanguage() const;
    vk::ShaderStageFlagBits GetVkShaderStageFlagBits() const;
};

#endif // Shader_hpp__