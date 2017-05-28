#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include "glslang/SPIRV/spirv.hpp"
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
    shaderSource.clear();
    binaryShader.clear();
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
    std::fstream shader_file(shader_path);

    if (shader_file.is_open())
    {
        shaderSource.resize(filesystem::file_size(shader_path)); // working
        shader_file.read(shaderSource.data(), filesystem::file_size(shader_path)); // working
        sourceFound = true;
    }
    shader_file.close();
}

void ShaderWorker::LoadBinaryShader()
{
    filesystem::path shader_path = Core.BinaryShadersPath.string() + shaderName + binaryExt;
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
            shader_file.open(shader_path);
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
        shader_file.open(shader_path);
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
    
    filesystem::path shader_source = Core.ShadersPath.string() + shaderName;
    filesystem::path saved_hash = Core.BinaryShadersPath.string() + shaderName + ".hash";

    // Check the current source file hash and the saved hash
    // TODO: Implement

    return false;
}

void ShaderWorker::CompileShader()
{
    filesystem::path shader_source = Core.ShadersPath.string() + shaderName;
    filesystem::path shader_binary = Core.BinaryShadersPath.string() + shaderName + binaryExt;
    filesystem::path saved_hash = Core.BinaryShadersPath.string() + shaderName + ".hash";

    if (Core.isGlobalDebug())
        Msg("ShaderWorker:: compiling: {}", shaderName);

    bool success = true;
    char* buf = shaderSource.data();
    char** buff = &buf; // what the hack

    EShMessages messages = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules | EShMsgAST);

    glslang::TProgram& program = *new glslang::TProgram;
    glslang::TShader* shader = new glslang::TShader(GetLanguage());
    shader->setStrings(buff, 1);

    std::string preprocessed;
    glslang::TShader::ForbidIncluder includer;
    success = shader->preprocess(&resources, 110, ENoProfile, false, false,
                                 messages, &preprocessed, includer);

    success = shader->parse(&resources, 110, false, messages);

    Msg("{}", shader->getInfoLog());
    Msg("{}", shader->getInfoDebugLog());

    if (success)
        program.addShader(shader);
    else
    {
        Msg("ShaderWorker:: failed to compile: {}", shaderName);
        return;
    }

    std::vector<unsigned int> spirv;
    spv::SpvBuildLogger logger;
    //if (program.getIntermediate(GetLanguage()))
        glslang::GlslangToSpv(*program.getIntermediate(GetLanguage()), spirv, &logger);
    Log(logger.getAllMessages());
    glslang::OutputSpvBin(spirv, shader_binary.string().c_str()); // string().c_str() what a nice costyl

    if (Core.isGlobalDebug() && success)
        Msg("ShaderWorker:: compiled: {}", shaderName);
    // Compile the shader - almost done
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
