#include "pch.hpp"

#include <fstream>
#include <filesystem>
namespace filesystem = std::filesystem;

#include <ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>

#include "xdCore/Core.hpp"
#include "xdCore/CommandLine.hpp"
#include "xdCore/Filesystem.hpp"
#include "Shader.hpp"

namespace XDay::CommandLine
{
Key KeyShaderForceRecompilation("shrec", "Force shader recompilation", KeyType::Boolean);
Key KeyShaderPreprocess("shpre", "Outputs preprocessed shaders to the shader sources dir. Works only in debug mode", KeyType::Boolean);
} // XDay::CommandLine

using namespace XDay;

ShaderWorker::ShaderWorker(std::string _name, const vk::UniqueDevice& _device, const TBuiltInResource& _resources)
    : shaderName(_name), device(_device), result(vk::Result::eNotReady), resources(_resources),
    binaryExt(".spv"), sourceFound(false), binaryFound(false)
{
    Initialize();
}

ShaderWorker::~ShaderWorker()
{
    shaderSource.clear();
    binaryShader.clear();
}

void ShaderWorker::Initialize()
{
    // Search for shader GLSL source
    LoadShader();

    // Search for binary SPIR-V shader
    LoadBinaryShader();

    CreateVkShaderModule();
    SetVkShaderStage();
}

void ShaderWorker::LoadShader()
{
    const auto shader_source_path = FS.ShadersPath.string() + shaderName;
    std::fstream shader_source_file(shader_source_path);

    if (shader_source_file.is_open())
    {
        shaderSource.resize(filesystem::file_size(shader_source_path));
        shader_source_file.read(shaderSource.data(), filesystem::file_size(shader_source_path));
        sourceFound = true;
    }
    shader_source_file.close();
}

void ShaderWorker::LoadBinaryShader()
{
    const auto shader_binary_path = FS.BinaryShadersPath.string() + shaderName + binaryExt;
    std::ifstream shader_binary_file(shader_binary_path, std::ios::binary);

    // check if file exist and it's not empty
    if (!CommandLine::KeyShaderForceRecompilation.IsSet() && shader_binary_file.is_open() && filesystem::file_size(shader_binary_path) != 0)
    {
        // Found shader, but it is old? Use it.
        // Recompile it and only then try to use new shader.
        binaryShader.resize(filesystem::file_size(shader_binary_path));
        shader_binary_file.read(binaryShader.data(), filesystem::file_size(shader_binary_path));
        binaryFound = true;

        binaryIsOld = CheckIfShaderChanged();

        if (binaryIsOld && !shaderSource.empty())
        {
            // Binary shader is old, but you have GLSL source? Try to recompile it.
            CompileShader();
            shader_binary_file.open(shader_binary_path, std::ios::binary);
            if (!shader_binary_file.is_open())
                Log::Warning("ShaderWorker::LoadBinaryShader():: Failed to open shader after recompilation."\
                        "Using old binary shader. Shader: {}", shaderName);
            else
            {
                binaryShader.resize(filesystem::file_size(shader_binary_path));
                shader_binary_file.read(binaryShader.data(), filesystem::file_size(shader_binary_path));
                binaryIsOld = false;
            }
        }
    }
    else if (!shaderSource.empty())
    {
        if (shader_binary_file.is_open())
            shader_binary_file.close();
        // Binary shader not found, but you have GLSL source? Try to compile it.
        CompileShader();
        shader_binary_file.open(shader_binary_path, std::ios::binary);
        if (!shader_binary_file.is_open())
            Log::Error("ShaderWorker::LoadBinaryShader():: Failed to open shader after compilation. Shader: {}", shaderName);
        else
        {
            binaryShader.resize(filesystem::file_size(shader_binary_path));
            shader_binary_file.read(binaryShader.data(), filesystem::file_size(shader_binary_path));
            binaryFound = true;
        }
    }
    else
        Log::Error("ShaderWorker::LoadBinaryShader():: Shader not found: {}", shaderName);

    shader_binary_file.close();
}

bool ShaderWorker::CheckIfShaderChanged()
{
    auto shader_source_path = FS.ShadersPath.string() + shaderName;
    auto shader_hash_path = FS.BinaryShadersPath.string() + shaderName + ".hash";

    // Check the current source file hash and the saved hash
    // TODO: Implement

    return false;
}

void ShaderWorker::CompileShader()
{
    glslang::InitializeProcess();

    const auto shader_source_path = FS.ShadersPath.string() + shaderName;
    const auto shader_binary_path = FS.BinaryShadersPath.string() + shaderName + binaryExt;
    const auto shader_hash_path = FS.BinaryShadersPath.string() + shaderName + ".hash";

    if (Core.isGlobalDebug())
        Log::Debug("ShaderWorker::CompileShader():: compiling: {}", shaderName);

    bool success = true;
    const char* source_bytes = shaderSource.data();

    EShMessages messages = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules | EShMsgAST);

    glslang::TProgram* program = new glslang::TProgram;
    glslang::TShader* shader = new glslang::TShader(GetLanguage());
    shader->setStrings(&source_bytes, 1);
    shader->setEntryPoint("main");

    if (Core.isGlobalDebug() && CommandLine::KeyShaderPreprocess.IsSet())
    {
        std::string preprocessed;
        glslang::TShader::ForbidIncluder includer;
        success = shader->preprocess(&resources, 450, ENoProfile, false, false,
                                     messages, &preprocessed, includer);
        if (success)
        {
            std::ofstream pp_source_file(shader_source_path + ".pp");
            pp_source_file << preprocessed;
            Log::Debug("ShaderWorker::CompileShader():: shader preprocessed: {}", shaderName);
        }
        else
            Log::Error("ShaderWorker::CompileShader():: failed to preprocess shader: {}", shaderName);
    }

    success = shader->parse(&resources, 450, false, messages);

    if (!success)
        Log::Error("ShaderWorker::CompileShader():: info: {}", shader->getInfoLog());
    if (false && Core.isGlobalDebug())
        Log::Debug("ShaderWorker::CompileShader():: debug info: {}", shader->getInfoDebugLog());

    if (success)
        program->addShader(shader);
    else
    {
        Log::Error("ShaderWorker::CompileShader():: failed to compile: {}", shaderName);
        delete program;
        delete shader;
        return;
    }

    success = program->link(messages);
    if (!success)
    {
        Log::Error("ShaderWorker::CompileShader():: failed to compile: {}", shaderName);
        delete program;
        delete shader;
        return;
    }

    program->mapIO();
    program->buildReflection();

    std::vector<unsigned int> spirv;
    spv::SpvBuildLogger logger;

    glslang::GlslangToSpv(*program->getIntermediate(GetLanguage()), spirv, &logger);

    if (Core.isGlobalDebug() && !logger.getAllMessages().empty())
        Log::Warning("ShaderWorker::CompileShader()::GlslangToSpv():: {}", logger.getAllMessages());

    glslang::OutputSpvBin(spirv, shader_binary_path.c_str());

    if (Core.isGlobalDebug() && filesystem::file_size(shader_binary_path) != 0)
        Log::Debug("ShaderWorker::CompileShader():: compiled: {}", shaderName);

    delete program;
    delete shader;

    glslang::FinalizeProcess();

    // Get current source hash and save it
    // TODO: Implement
}

void ShaderWorker::CreateVkShaderModule()
{
    auto size = binaryShader.size() / sizeof(uint32_t) + 1;

    std::vector<uint32_t> codeAligned(size);
    memcpy(codeAligned.data(), binaryShader.data(), binaryShader.size());

    vk::ShaderModuleCreateInfo smInfo({}, binaryShader.size(), codeAligned.data());

    shaderModule = device->createShaderModuleUnique(smInfo);

    if (!shaderModule)
        Log::Error("ShaderWorker::CreateVkShaderModule():: failed to create vulkan shader module for: {}", shaderName);
}

void ShaderWorker::SetVkShaderStage()
{
    stageInfo.setStage(GetVkShaderStageFlagBits());
    stageInfo.setModule(*shaderModule);
    stageInfo.setPName("main");
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

vk::ShaderModule ShaderWorker::GetShaderModule() const
{
    return *shaderModule;
}

vk::PipelineShaderStageCreateInfo ShaderWorker::GetVkShaderStageInfo() const
{
    return stageInfo;
}


EShLanguage ShaderWorker::GetLanguage() const
{
    if (shaderName.find(".vert") != std::string::npos)
        return EShLangVertex;
    if (shaderName.find(".tesc") != std::string::npos)
        return EShLangTessControl;
    if (shaderName.find(".tese") != std::string::npos)
        return EShLangTessEvaluation;
    if (shaderName.find(".geom") != std::string::npos)
        return EShLangGeometry;
    if (shaderName.find(".frag") != std::string::npos)
        return EShLangFragment;
    if (shaderName.find(".comp") != std::string::npos)
        return EShLangCompute;

    Log::Error("ShaderWorker::GetLanguage():: cant find stage for {}", shaderName);
    return EShLanguage();
}

vk::ShaderStageFlagBits ShaderWorker::GetVkShaderStageFlagBits() const
{
    if (shaderName.find(".vert") != std::string::npos)
        return vk::ShaderStageFlagBits::eVertex;
    if (shaderName.find(".tesc") != std::string::npos)
        return vk::ShaderStageFlagBits::eTessellationControl;
    if (shaderName.find(".tese") != std::string::npos)
        return vk::ShaderStageFlagBits::eTessellationEvaluation;
    if (shaderName.find(".geom") != std::string::npos)
        return vk::ShaderStageFlagBits::eGeometry;
    if (shaderName.find(".frag") != std::string::npos)
        return vk::ShaderStageFlagBits::eFragment;
    if (shaderName.find(".comp") != std::string::npos)
        return vk::ShaderStageFlagBits::eCompute;

    Log::Error("ShaderWorker::GetVkShaderStageBits():: cant find stage for {}", shaderName);
    return vk::ShaderStageFlagBits::eAll;
}
