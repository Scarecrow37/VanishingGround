#include "pch.h"
#include "VertexShader.h"

HRESULT VertexShader::LoadResource(const std::filesystem::path& filePath)
{
    HRESULT hr = CompileShader(filePath.c_str(), "vs_main", "vs_5_1");
    FAILED_CHECK_BREAK(hr);

    return hr;
}
