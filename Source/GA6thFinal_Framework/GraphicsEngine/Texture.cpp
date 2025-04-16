#include "pch.h"
#include "Texture.h"
#include <DirectXTex.h>

Texture::~Texture()
{
    Free();
}

HRESULT Texture::LoadResource(const std::filesystem::path& filePath)
{
    ID3D11Device* pDevice = g_pGraphicDevice->GetDevice();
    std::filesystem::path file = filePath;

    std::wstring extension = file.extension().wstring();
    ScratchImage image;
    TexMetadata metadata;

    if (L".dds" == extension)
    {
        FAILED_CHECK_BREAK(DirectX::LoadFromDDSFile(filePath.c_str(), DirectX::DDS_FLAGS_NONE, &metadata, image));
    }
    else if (L".tga" == extension)
    {
        FAILED_CHECK_BREAK(DirectX::LoadFromTGAFile(filePath.c_str(), &metadata, image));
    }
    else
    {
        FAILED_CHECK_BREAK(DirectX::LoadFromWICFile(filePath.c_str(), DirectX::WIC_FLAGS_IGNORE_SRGB, &metadata, image));
    }

    FAILED_CHECK_BREAK(DirectX::CreateShaderResourceView(pDevice, image.GetImages(), image.GetImageCount(), metadata, &_pTexture));

    _size = { (float)metadata.width, (float)metadata.height };

    return S_OK;
}

void Texture::Free()
{
    SafeRelease(_pTexture);
}
