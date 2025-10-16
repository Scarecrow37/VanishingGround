#pragma once
#include "ParticleHelper.h"

// 전방 선언
class Texture;

class ParticleRenderModule
{
public:
    virtual ~ParticleRenderModule();

    // 경로 수명 보존을 위해 값 복사로 보관
    void                SetModelAndTexturePath(const std::wstring& value) { _modelAndTexturePath = value; }
    const std::wstring& GetModelAndTexturePath() const { return _modelAndTexturePath; }

    virtual class SpriteModule*       AsSprite() { return nullptr; }
    virtual const class SpriteModule* AsSprite() const { return nullptr; }
    virtual class RibbonModule*       AsRibbon() { return nullptr; }
    virtual const class RibbonModule* AsRibbon() const { return nullptr; }

protected:
    std::wstring _modelAndTexturePath = L"";
};

class SpriteModule : public ParticleRenderModule
{
public:
    virtual ~SpriteModule() override;

    void SetFrameInfo(Vector4 frameInfo);
    void SetFrameInfo(int widthCount, int heightCount, int startIndex, int totalCount);

    // 텍스처 로딩/변경
    void LoadAlbedoTexture(std::wstring_view filePath);
    void ChangeAlbedoTexture(std::wstring_view filePath);
    void SetAlbedoTexture(std::shared_ptr<Texture> texture);

    Vector4                            GetInitialFrameInfo() const;
    UINT                               GetAlbedoTextureID() const noexcept;
    const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandle() const { return _albedoTexture->GetGPUHandle(); }
    void                               CalculateFrameInfos();

    SpriteModule*       AsSprite() override { return this; }
    const SpriteModule* AsSprite() const override { return this; }

protected:

    std::shared_ptr<Texture> _albedoTexture;
    Vector4                  _initialFrameInfo;
    std::vector<Vector4>     _preCalculatedFrameInfos;
    UMPARTICLE_PROPERTY(std::wstring, _newAlbedoTexturePath, NewAlbedoTexturePath, L"");
    UMPARTICLE_PROPERTY(bool, _isAlbedoTextureChanged, TextureChangeFlag, false);
    UMPARTICLE_PROPERTY(float, _frameDuration, FrameDuration, 1 / 24.f);
};

class MeshModule : public ParticleRenderModule
{
};

class RibbonModule : public ParticleRenderModule
{
public:
    virtual ~RibbonModule() override;

    void LoadAlbedoTexture(std::wstring_view filePath);
    void SetAlbedoTexture(std::shared_ptr<Texture> texture);
    void ChangeAlbedoTexture(std::wstring_view filePath);

    UINT GetAlbedoTextureID() const;
    const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandle() const { return _albedoTexture->GetGPUHandle(); }

    RibbonModule*       AsRibbon() override { return this; }
    const RibbonModule* AsRibbon() const override { return this; }

protected:
    std::shared_ptr<Texture> _albedoTexture;
    UMPARTICLE_PROPERTY_REF(std::wstring, _newAlbedoTexturePath, NewAlbedoTexturePath, L"");
    UMPARTICLE_PROPERTY(bool, _isAlbedoTextureChanged, TextureChangeFlag, false);
    UMPARTICLE_PROPERTY_REF(Vector4, _startNormal, StartNormal, Vector4(0, 0, -1, 0));
    UMPARTICLE_PROPERTY_REF(Vector4, _endNormal, EndNormal, Vector4(0, 0, -1, 0));
    UMPARTICLE_PROPERTY_REF(Vector4, _ribbonVector, RibbonVector, Vector4(1, 0, 0, 0));
};

class MiscModule : public SpriteModule
{
public:
    ~MiscModule() override;

protected:
    /// distortion = 0, blur = 1,
    UMPARTICLE_PROPERTY(uint8_t, _bitFlag, BitFlag, 0);
};
