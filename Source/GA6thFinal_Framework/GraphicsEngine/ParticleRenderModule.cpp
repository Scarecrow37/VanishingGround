#include "pch.h"
#include "ParticleRenderModule.h"
#include "Model.h"
void SpriteModule::Initialize() {}

void RibbonModule::Initialize() {}

SpriteModule::~SpriteModule()
{
    _albedoTexture = nullptr;
}

void SpriteModule::SetFrameInfo(Vector4 frameInfo)
{
    _initialFrameInfo = frameInfo;
    CalculateFrameInfos();
}

void SpriteModule::SetFrameInfo(int widthCount, int heightCount, int startIndex, int totalCount)
{
    _initialFrameInfo =
        Vector4((float)widthCount, (float)heightCount, (float)startIndex, static_cast<float>(totalCount));
    CalculateFrameInfos();
}

DirectX::SimpleMath::Vector4 SpriteModule::GetInitialFrameInfo() const
{
    return _initialFrameInfo;
}

Texture* SpriteModule::GetNormalTexture() const
{
    return _normalTexture.get();
}

void SpriteModule::CalculateFrameInfos()
{
    _preCalculatedFrameInfos.clear();
    Vector2 offset = {1.f / _initialFrameInfo.x, 1 / _initialFrameInfo.y};
    for (int i = 0; i < _initialFrameInfo.w; ++i)
    {

        Vector4 newFrame = {0, 0, 1, 1};
        UINT    x        = i % (UINT)_initialFrameInfo.x;
        UINT    y        = i / (UINT)_initialFrameInfo.x;

        newFrame.x = x * offset.x;
        newFrame.y = y * offset.y;
        newFrame.z = newFrame.x + offset.x;
        newFrame.w = newFrame.y + offset.y;
        _preCalculatedFrameInfos.push_back(newFrame);
    }
}

RibbonModule::~RibbonModule()
{
    _albedoTexture = nullptr;
}

void SpriteModule::SetAlbedoTexture(std::shared_ptr<class Texture> texture)
{
    _albedoTexture = std::move(texture);
}

void RibbonModule::SetAlbedoTexture(std::shared_ptr<class Texture> texture)
{
    _albedoTexture = std::move(texture);
}

void SpriteModule::ChangeAlbedoTexture(std::wstring_view filePath)
{
    _isAlbedoTextureChanged = true;
    _modelAndTexturePath    = filePath;
}

void RibbonModule::ChangeAlbedoTexture(std::wstring_view filePath)
{
    _isAlbedoTextureChanged = true;
    _modelAndTexturePath    = filePath;
}

Texture* SpriteModule::GetAlbedoTexture() const
{
    return _albedoTexture.get();
}

Texture* RibbonModule::GetAlbedoTexture() const
{
    return _albedoTexture.get();
}
