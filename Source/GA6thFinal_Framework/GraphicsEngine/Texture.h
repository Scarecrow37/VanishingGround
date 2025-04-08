#pragma once
#include "Resource.h"

class Texture : public Resource
{
public:
    explicit Texture() = default;
    virtual ~Texture();

public:
    HRESULT LoadResource(const std::filesystem::path& filePath) override;
    ID3D11ShaderResourceView** Get() { return &_pTexture; }
    Vector2 GetSize() const { return _size; }

private:
    // Resource을(를) 통해 상속됨
    void Free() override;

private:
    ID3D11ShaderResourceView*   _pTexture = nullptr;
    Vector2                     _size;
};