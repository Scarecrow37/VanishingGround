#include "pch.h"
#include "ComputeShader.h"

HRESULT ComputeShader::LoadResource(const std::filesystem::path& filePath)
{
    HRESULT hr = CompileShader(filePath.c_str(), "cs_main", "cs_5_1");
    FAILED_CHECK_BREAK(hr);

    return hr;
}
