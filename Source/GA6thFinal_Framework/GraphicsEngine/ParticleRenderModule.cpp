#include "pch.h"
#include "Model.h"
#include "ParticleRenderModule.h"

ParticleRenderModule::~ParticleRenderModule() = default;
SpriteModule::~SpriteModule() = default;
RibbonModule::~RibbonModule() = default;

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

void SpriteModule::CalculateFrameInfos() 
{
    _preCalculatedFrameInfos.clear();
    _frameDuration = _frameInfo.w / _frameInfo.z;
    float uOffset = 1 / _frameInfo.x;
    float vOffset = 1 / _frameInfo.y;
    UINT  uCount  = static_cast<UINT>(_frameInfo.x);
    for (int i = 0; i < _frameInfo.z; i++)
    {
        Vector4 uvMinMax = Vector4::Zero;
        uvMinMax.x       = (i % uCount) * uOffset;     // u min
        uvMinMax.y       = (i % uCount + 1) * uOffset; // u max
        uvMinMax.z       = (i / uCount) * vOffset;     // v min
        uvMinMax.w       = (i / uCount + 1) * vOffset; // v max
        _preCalculatedFrameInfos.push_back(uvMinMax);
    }
}

const DirectX::SimpleMath::Vector4& SpriteModule::GetCurrentFrameInfo(UINT index)
{
    return _preCalculatedFrameInfos[index];
}

UINT RibbonModule::GetAlbedoTextureID() const
{
    return _albedoTexture->GetID();
}

