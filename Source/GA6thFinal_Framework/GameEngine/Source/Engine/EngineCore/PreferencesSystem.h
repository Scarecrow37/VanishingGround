#pragma once

class PreferencesSystem : public File::FileEventSubscriber
{
public:
    PreferencesSystem();
    ~PreferencesSystem();

public:
    enum class TextureQuality
    {
        LOW,
        MEDIUM,
        HIGH,
    };

public:
    void Initialize();

public:
    // Graphics Setting
    void SetSSR(bool enable);
    void SetSSR(std::string_view sceneName, bool enable);

    void SetSSAO(bool enable);
    void SetSSAO(std::string_view sceneName, bool enable);

    void SetVolumetricFog(bool enable);
    void SetVolumetricFog(std::string_view sceneName, bool enable);

    void SetBloom(bool enable);
    void SetBloom(std::string_view sceneName, bool enable);

    void SetRayTracing(bool enable);
    void SetRayTracing(std::string_view sceneName, bool enable);

    void SetTextureQuality(TextureQuality quality);
    void SetTextureQuality(std::string_view sceneName, TextureQuality quality);
    // 작업해야함 구상 안함.
    void SetShadowQuality(int quality);
    void SetShadowQuality(std::string_view sceneName, int quality);

    // Audio Setting
public:
    bool IsSSR() { return _onSSR; }
    bool IsSSAO() { return _onSSAO; }
    bool IsBloom() { return _onBloom; }
    bool IsVolumFog() { return _onVolumFog; }
    int  GetTextureQuality() { return _textureQuality; }

public:
    void OnPostRequestedSave() override;
    void OnPostRequestedLoad() override;

private:
    std::unique_ptr<class GraphicsController> _graphicsController;

private:
    bool _onSSR          = true;
    bool _onSSAO         = true;
    bool _onBloom        = true;
    bool _onVolumFog     = true;
    int  _textureQuality = 2;
};
