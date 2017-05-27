#pragma once
#ifndef Shader_hpp__
#define Shader_hpp__
#include "Common/Platform.hpp" // this must be first
#include <vulkan/vulkan.hpp>

class ShaderWorker // TODO: wip name; rename it, maybe.
{
    std::string shaderName;
    vk::ShaderModule shader;
    const vk::Device& device;

    std::vector<char> shaderSource;
    std::vector<char> binaryShader;

    bool sourceFound;
    bool binaryFound;
    bool binaryIsOld;

public:
    ShaderWorker(std::string _name, const vk::Device& _device);
    void Initialize();

    bool isSourceFound() const;
    bool isBinaryFound() const;
    bool isBinaryOld() const;

private:
    void LoadShader();
    void LoadBinaryShader();
    bool CheckIfShaderChanged();
    void CompileShader();
};

#endif // Shader_hpp__