#include "pch.h"
#include "GeometryShader.h"

GeometryShader::GeometryShader()
{
    _type = ShaderType::GEOMETRY;
}

void GeometryShader::LoadResource(const std::filesystem::path& filePath, const std::function<void()>& callback)
{
    CompileShader(filePath.c_str(), "gs_main", "gs_5_1");
}
