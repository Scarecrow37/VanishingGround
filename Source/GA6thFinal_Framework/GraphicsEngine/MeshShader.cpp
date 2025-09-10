#include "pch.h"
#include "MeshShader.h"

void MeshShader::LoadResource(const std::filesystem::path& filePath, const std::function<void()>& callback)
{
    CompileShader(filePath.c_str(), "ms_main", "ms_5_1");
}
