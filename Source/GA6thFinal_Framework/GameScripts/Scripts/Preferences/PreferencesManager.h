#pragma once
#include "Utility/SingletonHelper.h"

constexpr int                        MaxVolume      = 10;
constexpr std::array<const char*, 4> GraphicsOptions = {"SSR", "SSAO", "Bloom", "VolumetricFog"};
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
    void LateUpdate() override;

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
    void OnPreferencesWindow(UINavigationComponent* backComponent);
    void GoToMainMenu();
    bool IsOpen() { return _opened; }

private:
    void OnPreferencesWindow(const Input::Controller&);
    void OffAbandonButtonComponent();
    void OffPreferencsButtonComponent();
    void OnAbandonButtonComponent();
    void OnPreferencsButtonComponent();

public:
    REFLECT_PROPERTY(MainMenuScene)
    GETTER_ONLY(std::string, MainMenuScene) 
    {
        File::Guid guid = ReflectFields->MainMenuSceneStr;
        return guid.ToPath().string(); 
    }
    PROPERTY(MainMenuScene)

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::string MainMenuSceneStr;
    REFLECT_FIELDS_END(PreferencesManager)

private:
    bool                               _isOpen      = false;
    bool                               _opened      = false;
    bool                               _openedDirty = false;
    bool                               _isOpenDirty = false;
    GameObject*                        _preferencesPannel = nullptr;
    std::map<std::string, GameObject*> _graphicsOption;
    std::map<std::string, float>       _soundOption;

    std::vector<Component*> _preferencesButtons;
    std::vector<Component*> _abandonButtons;
    bool                    _isOpenAbandonButton = false;
    bool                    _isOpenAbandonDirty  = false;
    bool                    _changeMainMenuSceneDirty = false;
    UINavigationComponent*  _backComponent            = nullptr;

private:
    SingletonComponent<PreferencesManager> _singletonComponent{this};

};
