#include "pch.h"
#include "PixelShader.h"

HRESULT PixelShader::LoadResource(const std::filesystem::path& filePath)
{
    HRESULT hr = CompileShader(filePath.c_str(), "ps_main", "ps_5_1");
    FAILED_CHECK_BREAK(hr);

    return hr;
}
