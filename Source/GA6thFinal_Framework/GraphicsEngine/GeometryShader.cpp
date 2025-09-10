#include "pch.h"
#include "GeometryShader.h"

void GeometryShader::LoadResource(const std::filesystem::path& filePath, const std::function<void()>& callback)
{
    CompileShader(filePath.c_str(), "gs_main", "gs_5_1");
}
