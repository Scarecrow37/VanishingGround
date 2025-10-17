#include "pch.h"
#include "ComputeShader.h"

ComputeShader::ComputeShader()
{
    _type = ShaderType::COMPUTE;
}

void ComputeShader::LoadResource(const std::filesystem::path& filePath, const std::function<void()>& callback)
{
    CompileShader(filePath.c_str(), "cs_main", "cs_5_1");
}
