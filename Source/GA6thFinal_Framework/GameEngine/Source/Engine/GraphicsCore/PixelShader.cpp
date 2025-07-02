#include "pch.h"
#include "PixelShader.h"

void PixelShader::LoadResource(const std::filesystem::path& filePath)
{
    CompileShader(filePath.c_str(), "ps_main", "ps_5_1");
}