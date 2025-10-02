#pragma once
#include "ParticleHelper.h"
/// <summary>
/// <para>something that is needed for rendering.</para>
/// <para>it can be sprite, mesh, or ribbon.</para>
/// <para>should be filled with infos that has something to do with DX12.</para>
/// <para>for example, textures for sprites, mesh vertices/indices for meshes, tessellation factors for ribbons.</para>
/// </summary>
class ParticleRenderModule
{
public:
    virtual ~ParticleRenderModule() {};
    virtual void      Initialize() {};
    void              SetModelAndTexturePath(std::wstring_view value) { _modelAndTexturePath = value.data(); }
    std::wstring_view GetModelAndTexturePath() { return std::wstring_view(_modelAndTexturePath); }

protected:
    std::wstring _modelAndTexturePath = L"";
};

class SpriteModule : public ParticleRenderModule
{
public:
    virtual ~SpriteModule();

    void Initialize() override;
    void SetFrameInfo(Vector4 frameInfo);
    void SetFrameInfo(int widthCount, int heightCount, int startIndex, int totalCount);
    void LoadAlbedoTexture(std::wstring_view filePath);
    void ChangeAlbedoTexture(std::wstring_view filePath);
    void SetAlbedoTexture(std::shared_ptr<class Texture> texture);

    Vector4        GetInitialFrameInfo() const;
    class Texture* GetAlbedoTexture() const;
    class Texture* GetNormalTexture() const;

protected:
    void CalculateFrameInfos();

    Vector4                        _initialFrameInfo;
    std::shared_ptr<class Texture> _albedoTexture;
    std::shared_ptr<class Texture> _normalTexture;
    std::vector<Vector4>           _preCalculatedFrameInfos;
    UMPARTICLE_PROPERTY(std::wstring, _newAlbedoTexturePath, NewAlbedoTexturePath, L"");
    UMPARTICLE_PROPERTY(bool, _isAlbedoTextureChanged, TextureChangeFlag, false);
    UMPARTICLE_PROPERTY(float, _frameDuration, FrameDuration, 1 / 24.f);
};

class MeshModule : public ParticleRenderModule
{

public:
    void Initialize() override {};
};

class RibbonModule : public ParticleRenderModule
{
public:
    virtual ~RibbonModule();

    void Initialize() override;
    void LoadAlbedoTexture(std::wstring_view filePath);
    void SetAlbedoTexture(std::shared_ptr<class Texture> texture);
    void ChangeAlbedoTexture(std::wstring_view filePath);

    class Texture* GetAlbedoTexture() const;

protected:
    std::shared_ptr<class Texture> _albedoTexture;
    std::shared_ptr<class Texture> _normalTexture;
    UMPARTICLE_PROPERTY_REF(std::wstring, _newAlbedoTexturePath, NewAlbedoTexturePath, L"");
    UMPARTICLE_PROPERTY(bool, _isAlbedoTextureChanged, TextureChangeFlag, false);
    UMPARTICLE_PROPERTY_REF(Vector4, _startNormal, StartNormal, Vector4(0, 0, -1, 0));
    UMPARTICLE_PROPERTY_REF(Vector4, _endNormal, EndNormal, Vector4(0, 0, -1, 0));
    UMPARTICLE_PROPERTY_REF(Vector4, _ribbonVector, RibbonVector, Vector4(1, 0, 0, 0));
};

class MiscModule : public SpriteModule
{
public:
    virtual ~MiscModule();

protected:
    /// <summary>
    /// distortion = 0, blur = 1,
    /// </summary>
    UMPARTICLE_PROPERTY(uint8_t, _bitFlag, BitFlag, 0);
};