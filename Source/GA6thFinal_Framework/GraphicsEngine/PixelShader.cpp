#include "pch.h"
#include "PixelShader.h"

PixelShader::PixelShader()
{
    _type = ShaderType::PIXEL;
}

void PixelShader::LoadResource(const std::filesystem::path& filePath, const std::function<void()>& callback)
{
    CompileShader(filePath.c_str(), "ps_main", "ps_5_1");
}