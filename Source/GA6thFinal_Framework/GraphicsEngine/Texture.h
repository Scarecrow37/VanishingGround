#pragma once
#include "Resource.h"

class Texture : public Resource
{
public:
    Texture();
    virtual ~Texture();

public:
    const SIZE& GetSize() const { return _size; }
    bool        IsValid() const override;

public:
    void SetResource(ID3D12Resource* resource);

public:
    void CreateShaderResourceView();

public:
    // Resource을(를) 통해 상속됨
    void LoadResource(const std::filesystem::path& filePath, const std::function<void()>& callback = nullptr) override;

private:
    DescriptorHandles _textureHandle;
    SIZE              _size;
};
