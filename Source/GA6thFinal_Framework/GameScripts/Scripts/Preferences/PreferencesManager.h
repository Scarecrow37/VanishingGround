#pragma once
constexpr int                        MaxVolume      = 10;
constexpr std::array<const char*, 4> GrphicsOptions = {"SSR", "SSAO", "Bloom", "VolumetricFog"};
constexpr std::array<const char*, 3> VolumeOptions  = {"MasterVolume", "BGMVolume", "SFXVolume"};
class PreferencesManager : public Component, public InputReceiver
{
    USING_PROPERTY(PreferencesManager)
public:
    PreferencesManager();
    ~PreferencesManager() override;

public:
    void Reset() override;
    void Awake() override;
    void Update() override;

public:
    void SetGraphicsOptions(std::string_view option, bool enable);
    void SetGraphicsQuality(PreferencesSystem::TextureQuality quality);

    // master, BGM, SFX
    void SetVolume(std::string_view option, int value);
    void OffPreferencesWindow();


public:
    void AddPreferencesButton(Component* comps);
    void AddAbandonButton(Component* comps);
    void OpenAbadonButtons();
    void CloseAbandonButtons();

private:
    void OnPreferencesWindow(const Input::Controller&);
    void OffAbandonButtonComponent();
    void OffPreferencsButtonComponent();
    void OnAbandonButtonComponent();
    void OnPreferencsButtonComponent();

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(PreferencesManager)

private:
    bool                               _isOpen      = false;
    bool                               _isOpenDirty = false;
    GameObject*                        _preferencesPannel;
    std::map<std::string, GameObject*> _graphicsOption;
    std::map<std::string, float>       _soundOption;

    std::vector<Component*> _preferencesButtons;
    std::vector<Component*> _abandonButtons;
    bool                    _isOpenAbandonButton = false;
    bool                    _isOpenAbandonDirty  = false;
};
