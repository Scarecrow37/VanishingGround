#include "pch.h"
#include "Shader.h"

HRESULT Shader::CompileShader(std::wstring_view filePath, std::string_view entry, std::string_view shaderModel)
{
	HRESULT hr = S_OK;
	ComPtr<ID3DBlob> error;

    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES;

#ifdef _DEBUG
    flags |= D3DCOMPILE_DEBUG| D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    hr = D3DCompileFromFile(filePath.data(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry.data(),
                            shaderModel.data(), flags, 0, _blob.GetAddressOf(), error.GetAddressOf());

    if (nullptr != error)
    {
        std::filesystem::path errorMessage = static_cast<const char*>(error->GetBufferPointer());
        ASSERT(SUCCEEDED(hr), errorMessage.c_str());
    }

    FAILED_CHECK_BREAK(hr);

	return hr;
}