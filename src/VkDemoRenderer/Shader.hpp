#pragma once

#include <vulkan/vulkan.hpp>
#include <ShaderLang.h>

namespace XDay
{
class ShaderWorker
{
    std::string shaderName;
    std::string binaryExt;
    const vk::UniqueDevice& device;
    const TBuiltInResource& resources;
    vk::UniqueShaderModule shaderModule;
    vk::PipelineShaderStageCreateInfo stageInfo;
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
    vk::Result result;

    std::vector<char> shaderSource;
    std::vector<char> binaryShader;

    bool sourceFound;
    bool binaryFound;
    bool binaryIsOld;

public:
    ShaderWorker(std::string _name, const vk::UniqueDevice& _device, const TBuiltInResource& _resources);
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
}
