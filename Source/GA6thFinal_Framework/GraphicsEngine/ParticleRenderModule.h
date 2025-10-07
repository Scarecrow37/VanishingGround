#pragma once
#include "ParticleHelper.h"

// 전방 선언
class Texture;

class ParticleRenderModule
{
public:
    virtual ~ParticleRenderModule() {}

    // 필요 시 오버라이드
    virtual void Initialize() {}

    // 경로 수명 보존을 위해 값 복사로 보관
    void                SetModelAndTexturePath(std::wstring value) { _modelAndTexturePath = std::move(value); }
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
    ~SpriteModule() override;

    void Initialize() override;
    void SetFrameInfo(Vector4 frameInfo);
    void SetFrameInfo(int widthCount, int heightCount, int startIndex, int totalCount);

    // 텍스처 로딩/변경
    void LoadAlbedoTexture(std::wstring_view filePath);
    void ChangeAlbedoTexture(std::wstring_view filePath);
    void SetAlbedoTexture(std::shared_ptr<Texture> texture);

    Vector4  GetInitialFrameInfo() const;
    Texture* GetAlbedoTexture() const;
    Texture* GetNormalTexture() const;

    SpriteModule*       AsSprite() override { return this; }
    const SpriteModule* AsSprite() const override { return this; }

protected:
    void CalculateFrameInfos();

    Vector4                  _initialFrameInfo;
    std::shared_ptr<Texture> _albedoTexture;
    std::shared_ptr<Texture> _normalTexture;
    std::vector<Vector4>     _preCalculatedFrameInfos;
    UMPARTICLE_PROPERTY(std::wstring, _newAlbedoTexturePath, NewAlbedoTexturePath, L"");
    UMPARTICLE_PROPERTY(bool, _isAlbedoTextureChanged, TextureChangeFlag, false);
    UMPARTICLE_PROPERTY(float, _frameDuration, FrameDuration, 1 / 24.f);
};

class MeshModule : public ParticleRenderModule
{
public:
    void Initialize() override {}
};

class RibbonModule : public ParticleRenderModule
{
public:
    ~RibbonModule() override;

    void Initialize() override;
    void LoadAlbedoTexture(std::wstring_view filePath);
    void SetAlbedoTexture(std::shared_ptr<Texture> texture);
    void ChangeAlbedoTexture(std::wstring_view filePath);

    Texture* GetAlbedoTexture() const;

    RibbonModule*       AsRibbon() override { return this; }
    const RibbonModule* AsRibbon() const override { return this; }

protected:
    std::shared_ptr<Texture> _albedoTexture;
    std::shared_ptr<Texture> _normalTexture;
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
