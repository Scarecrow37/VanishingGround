#include "pch.h"
#include "Model.h"
#include "ParticleRenderModule.h"

ParticleRenderModule::~ParticleRenderModule() = default;
SpriteModule::~SpriteModule() = default;
RibbonModule::~RibbonModule() = default;

void SpriteModule::SetFrameInfo(Vector4 frameInfo)
{
    _initialFrameInfo = frameInfo;
}

void SpriteModule::SetFrameInfo(int widthCount, int heightCount, int startIndex, int totalCount)
{
    _initialFrameInfo = Vector4((float)widthCount, (float)heightCount, (float)startIndex, (float)totalCount);
}

Vector4 SpriteModule::GetInitialFrameInfo() const
{
    return _initialFrameInfo;
}

void SpriteModule::SetAlbedoTexture(std::shared_ptr<Texture> texture)
{
    _albedoTexture = std::move(texture);
}

void RibbonModule::SetAlbedoTexture(std::shared_ptr<Texture> texture)
{
    _albedoTexture = std::move(texture);
}

void SpriteModule::ChangeAlbedoTexture(std::wstring_view filePath)
{
    _isAlbedoTextureChanged = true;
    _modelAndTexturePath    = std::wstring(filePath); // 안전 복사
}

void RibbonModule::ChangeAlbedoTexture(std::wstring_view filePath)
{
    _isAlbedoTextureChanged = true;
    _modelAndTexturePath    = std::wstring(filePath); // 안전 복사
}

UINT SpriteModule::GetAlbedoTextureID() const noexcept
{
    return _albedoTexture->GetID(); 
}

UINT RibbonModule::GetAlbedoTextureID() const
{
    return _albedoTexture->GetID();
}

