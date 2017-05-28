#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>

namespace filesystem = std::experimental::filesystem::v1;

#include <ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>

#include "xdEngine/xdCore.hpp"
#include "Shader.hpp"
#include "Debug/Log.hpp"

ShaderWorker::ShaderWorker(std::string _name, const vk::Device& _device, const TBuiltInResource& _resources)
    : shaderName(_name), device(_device), resources(_resources),
    binaryExt(".spv"), sourceFound(false), binaryFound(false)
{
    Initialize();
}

ShaderWorker::~ShaderWorker()
{
    device.destroyShaderModule(shaderModule);
    shaderSource.clear();
    binaryShader.clear();
}

void ShaderWorker::Initialize()
{
    // Search for shader GLSL source
    LoadShader();

    // Search for binary SPIR-V shader
    LoadBinaryShader();

    CreateShaderModule();
}

void ShaderWorker::LoadShader()
{
    auto shader_path = Core.ShadersPath.string() + shaderName;
    std::fstream shader_file(shader_path);

    if (shader_file.is_open())
    {
        shaderSource.resize(filesystem::file_size(shader_path));
        shader_file.read(shaderSource.data(), filesystem::file_size(shader_path));
        sourceFound = true;
    }
    shader_file.close();
}

void ShaderWorker::LoadBinaryShader()
{
    auto shader_path = Core.BinaryShadersPath.string() + shaderName + binaryExt;
    std::ifstream shader_file(shader_path, std::ios::binary);

    if (shader_file.is_open() && filesystem::file_size(shader_path) != 0) // check if file exist and it's not empty
    {
        // Found shader, but it is old? Use it.
        // Recompile it and only then try to use new shader.
        binaryShader.resize(filesystem::file_size(shader_path));
        shader_file.read(binaryShader.data(), filesystem::file_size(shader_path));
        binaryFound = true;

        binaryIsOld = CheckIfShaderChanged();

        if (binaryIsOld && !shaderSource.empty())
        {
            // Binary shader is old, but you have GLSL source? Try to recompile it.
            CompileShader();
            shader_file.open(shader_path, std::ios::binary);
            if (!shader_file.is_open())
                Msg("Failed to open shader after recompilation. Using old binary shader. Shader: {}", shaderName)
            else
            {
                binaryShader.resize(filesystem::file_size(shader_path));
                shader_file.read(binaryShader.data(), filesystem::file_size(shader_path));
                binaryIsOld = false;
            }
        }
    }
    else if (!shaderSource.empty())
    {
        // Binary shader not found, but you have GLSL source? Try to compile it.
        CompileShader();
        shader_file.open(shader_path, std::ios::binary);
        if (!shader_file.is_open())
            Msg("Failed to open shader after compilation. Shader: {}", shaderName)
        else
        {
            binaryShader.resize(filesystem::file_size(shader_path));
            shader_file.read(binaryShader.data(), filesystem::file_size(shader_path));
            binaryFound = true;
        }
    }
    else
        Msg("Shader not found: {}", shaderName);

    shader_file.close();
}

bool ShaderWorker::CheckIfShaderChanged()
{
    
    auto shader_source = Core.ShadersPath.string() + shaderName;
    auto saved_hash = Core.BinaryShadersPath.string() + shaderName + ".hash";

    // Check the current source file hash and the saved hash
    // TODO: Implement

    return false;
}

void ShaderWorker::CompileShader()
{
    auto shader_source = Core.ShadersPath.string() + shaderName;
    auto shader_binary = Core.BinaryShadersPath.string() + shaderName + binaryExt;
    auto saved_hash = Core.BinaryShadersPath.string() + shaderName + ".hash";

    if (Core.isGlobalDebug())
        Msg("ShaderWorker:: compiling: {}", shaderName);

    bool success = true;
    const char* sourceBytes = shaderSource.data();

    EShMessages messages = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules | EShMsgAST);

    glslang::TProgram* program = new glslang::TProgram;
    glslang::TShader* shader = new glslang::TShader(GetLanguage());
    shader->setStrings(&sourceBytes, 1);
    shader->setEntryPoint("main");

    std::string preprocessed;
    glslang::TShader::ForbidIncluder includer;
    success = shader->preprocess(&resources, 450, ENoProfile, false, false,
                                 messages, &preprocessed, includer);

    success = shader->parse(&resources, 450, false, messages);

    Msg("{}", shader->getInfoLog());
    Msg("{}", shader->getInfoDebugLog());

    if (success)
        program->addShader(shader);
    else
    {
        Msg("ShaderWorker:: failed to compile: {}", shaderName);
        return;
    }

    success = program->link(messages);
    if (!success)
    {
        Msg("ShaderWorker:: failed to compile: {}", shaderName);
        return;
    }

    program->mapIO();
    program->buildReflection();

    std::vector<unsigned int> spirv;
    spv::SpvBuildLogger logger;

    glslang::GlslangToSpv(*program->getIntermediate(GetLanguage()), spirv, &logger);

    if (Core.isGlobalDebug())
    	Log(logger.getAllMessages());

    glslang::OutputSpvBin(spirv, shader_binary.c_str());

    if (Core.isGlobalDebug() && success)
        Msg("ShaderWorker:: compiled: {}", shaderName);

    // Get current source hash and save it
    // TODO: Implement
}

void ShaderWorker::CreateShaderModule()
{
    auto size = binaryShader.size() / sizeof(uint32_t) + 1;

    std::vector<uint32_t> codeAligned(size);
    memcpy(codeAligned.data(), binaryShader.data(), binaryShader.size());
    
    vk::ShaderModuleCreateInfo smInfo({}, binaryShader.size(), codeAligned.data());

    result = device.createShaderModule(&smInfo, nullptr, &shaderModule);

    if (!(result == vk::Result::eSuccess))
        Msg("ShaderWorker:: failed to create vulkan shader module for: {}", shaderName);

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
    return shaderModule;
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
    Msg("ShaderWorker:: cant find stage for {}", shaderName);
}


