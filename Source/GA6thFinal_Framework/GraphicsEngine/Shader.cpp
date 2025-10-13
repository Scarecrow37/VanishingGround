#include "pch.h"
#include "Shader.h"
#include "ShaderBytecode.generated.h"

void Shader::CompileShader(std::wstring_view filePath, std::string_view entry, std::string_view shaderModel)
{
#ifdef _DEBUG
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | 
                 D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES |
                 D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;

#ifdef _DEBUG
    flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ComPtr<ID3DBlob> error;
	HRESULT hr = S_OK;
    hr = D3DCompileFromFile(filePath.data(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry.data(),
                            shaderModel.data(), flags, 0, &_blob, &error);

    FAILED_CHECK_MESSAGE(SUCCEEDED(hr), L"Shader Compile Error!");

    if (nullptr != error)
    {
        std::filesystem::path errorMessage = static_cast<const char*>(error->GetBufferPointer());
        FAILED_CHECK_MESSAGE(hr, errorMessage.c_str());
    }
#else
    switch (_type)
    {
    case Shader::ShaderType::VERTEX:
        _byteCode = GE::GetShaderBytecode(GE::globalNameToVSEnumMap.at(filePath.data()));
        break;
    case Shader::ShaderType::PIXEL:
        _byteCode = GE::GetShaderBytecode(GE::globalNameToPSEnumMap.at(filePath.data()));
        break;
    case Shader::ShaderType::COMPUTE:
        _byteCode = GE::GetShaderBytecode(GE::globalNameToCSEnumMap.at(filePath.data()));
        break;
    /*case Shader::ShaderType::GEOMETRY:
        _byteCode = GE::GetShaderBytecode(GE::globalNameToGSEnumMap.at(filePath.data()));
        break;*/
    }
#endif
}