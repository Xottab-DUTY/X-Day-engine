#include <fstream>
#include <sstream>
#include <filesystem>
namespace filesystem = std::experimental::filesystem::v1;

#include <ShaderLang.h>

#include "xdEngine/xdCore.hpp"
#include "Shader.hpp"
#include "Debug/Log.hpp"

ShaderWorker::ShaderWorker(std::string _name, const vk::Device& _device, const TBuiltInResource& _resources) : shaderName(_name), device(_device), resources(_resources),
    sourceFound(false), binaryFound(false)
{
    shaderSource = new char*;
    binaryShader = new char*;
    Initialize();
}

ShaderWorker::~ShaderWorker()
{
    delete[] shaderSource;
    delete[] binaryShader;
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
        *shaderSource = new char[filesystem::file_size(shader_path)];
        std::vector<char> buf(filesystem::file_size(shader_path));
        shader_file.read(buf.data(), filesystem::file_size(shader_path));
        *shaderSource = std::move(buf.data());
        sourceFound = true;
    }
    shader_file.close();
}

void ShaderWorker::LoadBinaryShader()
{
    filesystem::path shader_path = Core.BinaryShadersPath.string() + shaderName;
    std::ifstream shader_file(shader_path, std::ios::binary);

    if (shader_file.is_open())
    {
        // Use found shader, even if it is old
        // Recompile it and only then try to use new shader
        *binaryShader = new char[filesystem::file_size(shader_path)];
        shader_file.read(*binaryShader, filesystem::file_size(shader_path));
        binaryFound = true;

        binaryIsOld = CheckIfShaderChanged();

        if (binaryIsOld && shaderSource != nullptr)
        {
            CompileShader();
            shader_file.open(shader_path);
            if (!shader_file.is_open())
                Msg("Failed to open shader after recompilation. Using old binary shader. Shader: {}", shaderName)
            else
            {
                *binaryShader = new char[filesystem::file_size(shader_path)];
                shader_file.read(*binaryShader, filesystem::file_size(shader_path));
                binaryIsOld = false;
            }
        }
    }
    else if (shaderSource != nullptr)
    {
        // Binary shader not found, but you have GLSL source? Try to compile it.
        CompileShader();
        shader_file.open(shader_path);
        if (!shader_file.is_open())
            Msg("Failed to open shader after compilation. Shader: {}", shaderName)
        else
        {
            *binaryShader = new char[filesystem::file_size(shader_path)];
            shader_file.read(*binaryShader, filesystem::file_size(shader_path));
            binaryFound = true;
        }
    }
    else
        Msg("Shader not found: {}", shaderName);

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

    EShMessages messages = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules);
    ShHandle compiler = ShConstructCompiler(GetLanguage(), 0);
    int result = ShCompile(compiler, shaderSource, 1, nullptr, EShOptSimple, &resources, 0, 110, false, messages);
    Msg("Shader compilation: {}", result);
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

EShLanguage ShaderWorker::GetLanguage() const
{
    if (shaderName.find(".vert"))
        return EShLangVertex;
    if (shaderName.find(".tesc"))
        return EShLangTessControl;
    if (shaderName.find(".tese"))
        return EShLangTessEvaluation;
    if (shaderName.find(".geom"))
        return EShLangGeometry;
    if (shaderName.find(".frag"))
        return EShLangFragment;
    if (shaderName.find(".comp"))
        return EShLangCompute;
}
