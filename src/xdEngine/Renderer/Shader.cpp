#include <fstream>
#include <filesystem>
namespace filesystem = std::experimental::filesystem::v1;

#include <ShaderLang.h>

#include "xdEngine/xdCore.hpp"
#include "Shader.hpp"
#include "Debug/Log.hpp"

ShaderWorker::ShaderWorker(std::string _name, const vk::Device& _device)
    : shaderName(_name), device(_device),
    sourceFound(false), binaryFound(false)
{
    Initialize();
}

void ShaderWorker::Initialize()
{
    // Search for shader GLSL source
    LoadShader();

    // Search for binary SPIR-V shader
    LoadBinaryShader();

    // ...
}

void ShaderWorker::LoadShader()
{
    filesystem::path shader_path = Core.ShadersPath.string() + shaderName;
    std::ifstream shader_file(shader_path);

    if (shader_file.is_open())
    {
        shader_file.read(shaderSource.data(), filesystem::file_size(shader_path));
        sourceFound = true;
        shader_file.close();
    }
}

void ShaderWorker::LoadBinaryShader()
{
    filesystem::path shader_path = Core.BinaryShadersPath.string() + shaderName;
    std::ifstream shader_file(shader_path, std::ios::binary);

    if (shader_file.is_open())
    {
        // Use found shader, even if it is old
        // Recompile it and only then try to use new shader
        shader_file.read(binaryShader.data(), filesystem::file_size(shader_path));
        binaryFound = true;

        binaryIsOld = CheckIfShaderChanged();

        if (binaryIsOld)
        {
            CompileShader();
            shader_file.open(shader_path);
            if (!shader_file.is_open())
                Msg("Failed to open shader after recompilation. Using old binary shader. Shader path: {}", shader_path.string())
            else
            {
                shader_file.read(binaryShader.data(), filesystem::file_size(shader_path));
                binaryIsOld = false;
            }
        }
    }
    else
    {
        // Shader not found? Compile it.
        CompileShader();
        shader_file.open(shader_path);
        if (!shader_file.is_open())
            Msg("Failed to open shader after compilation. Shader path: {}", shader_path.string())
        else
        {
            shader_file.read(binaryShader.data(), filesystem::file_size(shader_path));
            binaryFound = true;
        }
    }

    shader_file.close();
}

bool ShaderWorker::CheckIfShaderChanged()
{
    
    filesystem::path shader_source = Core.ShadersPath.string() + shaderName;
    filesystem::path saved_hash = Core.BinaryShadersPath.string() + shaderName + ".hash";

    // Check the current source file hash and the saved hash
    // TODO: Implement

    return false;
}

void ShaderWorker::CompileShader()
{
    filesystem::path shader_source = Core.ShadersPath.string() + shaderName;
    filesystem::path shader_binary = Core.BinaryShadersPath.string() + shaderName;
    filesystem::path saved_hash = Core.BinaryShadersPath.string() + shaderName + ".hash";

    // Compile shader from source
    // Put it in the binary shaders dir
    // Get current source hash and save it 
    // TODO: Implement
}

bool ShaderWorker::isSourceFound() const
{
    return sourceFound;
}

bool ShaderWorker::isBinaryFound() const
{
    return binaryFound;
}

bool ShaderWorker::isBinaryOld() const
{
    return binaryIsOld;
}
