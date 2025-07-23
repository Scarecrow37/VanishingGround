#pragma once
#include "Resource.h"

class Texture : public Resource
{
public:
    Texture()          = default;
    virtual ~Texture() = default;

public:
    const SIZE& GetSize() const { return _size; }

public:
    void SetResource(ID3D12Resource* resource);
    void CreateShaderResourceView();

public:
    // Resource을(를) 통해 상속됨
    void LoadResource(const std::filesystem::path& filePath) override;

private:
    SIZE _size;
};
