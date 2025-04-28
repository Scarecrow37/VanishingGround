#include "pch.h"
#include "GeometryShader.h"

HRESULT GeometryShader::LoadResource(const std::filesystem::path& filePath)
{
    HRESULT hr = CompileShader(filePath.c_str(), "gs_main", "gs_5_1");
    FAILED_CHECK_BREAK(hr);

    return hr;
}
