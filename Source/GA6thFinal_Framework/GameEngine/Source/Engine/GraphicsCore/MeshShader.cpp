#include "pch.h"
#include "MeshShader.h"

HRESULT MeshShader::LoadResource(const std::filesystem::path& filePath)
{
    HRESULT hr = CompileShader(filePath.c_str(), "ms_main", "ms_5_1");
    FAILED_CHECK_BREAK(hr);

    return hr;
}
