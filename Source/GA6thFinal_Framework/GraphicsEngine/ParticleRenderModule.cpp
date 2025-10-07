#include "pch.h"
#include "Model.h"
#include "ParticleRenderModule.h"

void SpriteModule::Initialize() {}
void RibbonModule::Initialize() {}

SpriteModule::~SpriteModule()
{
    _albedoTexture.reset();
}

RibbonModule::~RibbonModule()
{
    _albedoTexture.reset();
}

void SpriteModule::SetFrameInfo(Vector4 frameInfo)
{
    _initialFrameInfo = frameInfo;
    CalculateFrameInfos();
}

void SpriteModule::SetFrameInfo(int widthCount, int heightCount, int startIndex, int totalCount)
{
    _initialFrameInfo = Vector4((float)widthCount, (float)heightCount, (float)startIndex, (float)totalCount);
    CalculateFrameInfos();
}

Vector4 SpriteModule::GetInitialFrameInfo() const
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
    Vector2 offset = {1.f / _initialFrameInfo.x, 1.f / _initialFrameInfo.y};
    for (int i = 0; i < (int)_initialFrameInfo.w; ++i)
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

Texture* SpriteModule::GetAlbedoTexture() const
{
    return _albedoTexture.get();
}

Texture* RibbonModule::GetAlbedoTexture() const
{
    return _albedoTexture.get();
}
