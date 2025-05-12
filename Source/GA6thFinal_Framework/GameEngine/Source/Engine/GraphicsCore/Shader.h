#pragma once
#include "Resource.h"

class Shader : public Resource
{
public:
    Shader()          = default;
    virtual ~Shader() = default;

public:
    void* GetBufferPointer() const { return _blob->GetBufferPointer(); }
    SIZE_T GetBufferSize() const { return _blob->GetBufferSize(); }

protected:
    HRESULT CompileShader(std::wstring_view filePath, std::string_view entry, std::string_view shaderModel);

private:
    ComPtr<ID3DBlob> _blob;
};