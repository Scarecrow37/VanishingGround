#pragma once
#include "Resource.h"

class Shader : public Resource
{
protected:
    enum class ShaderType
    {
        VERTEX,
        PIXEL,
        GEOMETRY,
        COMPUTE
    };

public:
    Shader()          = default;
    virtual ~Shader() = default;

public:
#ifdef _DEBUG
    const void* GetBufferPointer() const { return _blob->GetBufferPointer(); }
    SIZE_T      GetBufferSize() const { return _blob->GetBufferSize(); }
#else
    const void* GetBufferPointer() const { return _byteCode.pShaderBytecode; }
    SIZE_T      GetBufferSize() const { return _byteCode.BytecodeLength; };
#endif

protected:
    void CompileShader(std::wstring_view filePath, std::string_view entry, std::string_view shaderModel);

protected:
    ShaderType _type;

private:
#ifdef _DEBUG
    ComPtr<ID3DBlob> _blob;
#else
    D3D12_SHADER_BYTECODE _byteCode;
#endif
};