#include "pch.h"
#include "VertexShader.h"

void VertexShader::LoadResource(const std::filesystem::path& filePath)
{
    CompileShader(filePath.c_str(), "vs_main", "vs_5_1");
}