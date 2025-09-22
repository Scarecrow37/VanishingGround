#include "pchScripts.h"
#include "PreferencesManager.h"
#include "PrefrencesWindow.h"

UMREAL_COMPONENT(PreferencesManager)

PreferencesManager::PreferencesManager()  = default;
PreferencesManager::~PreferencesManager() = default;

void PreferencesManager::Reset()
{
    BindInputAction(ControllerButton::BACK, Action::PRESSED, this, &PreferencesManager::OnPreferencesWindow);
}

void PreferencesManager::Awake()
{
    _preferencesPannel = GameObject::Find("PreferencesPannel").lock().get();
    if (nullptr == _preferencesPannel)
        UmLogger.Log(LogLevel::LEVEL_ERROR, "환경설정 패널이 없습니다!");
    else if (nullptr != _preferencesPannel)
        _preferencesPannel->SetActive(false);

    // 엔진에서 현재 저장된 설정값 적용
    SetGraphicsOptions("SSR", UmPreferences.IsSSR());
    SetGraphicsOptions("SSAO", UmPreferences.IsSSAO());
    SetGraphicsOptions("Bloom", UmPreferences.IsBloom());
    SetGraphicsOptions("VolumetricFog", UmPreferences.IsVolumFog());
    SetVolume("MasterVolume", static_cast<int>(UmPreferences.GetMasterVolume() * MaxVolume));
    SetVolume("BGMVolume", static_cast<int>(UmPreferences.GetBGMVolume() * MaxVolume));
    SetVolume("SFXVolume", static_cast<int>(UmPreferences.GetSFXVolume() * MaxVolume));
    switch (UmPreferences.GetTextureQuality())
    {
    case 0:
        SetGraphicsQuality(PreferencesSystem::TextureQuality::LOW);
        break;
    case 1:
        SetGraphicsQuality(PreferencesSystem::TextureQuality::MEDIUM);
        break;
    case 2:
        SetGraphicsQuality(PreferencesSystem::TextureQuality::HIGH);
        break;
    default:
        break;
    }
}

void PreferencesManager::Update()
{
    if (_isOpenDirty)
    {
        if (_isOpen)
        {
            if (nullptr == _preferencesPannel)
                UmLogger.Log(LogLevel::LEVEL_ERROR, "환경설정 패널이 없습니다!");
            else if (nullptr != _preferencesPannel)
                _preferencesPannel->SetActive(true);
            _isOpenDirty = false;
        }
        else
        {
            if (nullptr == _preferencesPannel)
                UmLogger.Log(LogLevel::LEVEL_ERROR, "환경설정 패널이 없습니다!");
            else if (nullptr != _preferencesPannel)
                _preferencesPannel->SetActive(false);
            _isOpenDirty = false;
        }
    }
    if (_isOpenAbandonDirty)
    {
        if (_isOpenAbandonButton)
        {
            OnAbandonButtonComponent();
            OffPreferencsButtonComponent();
        }
        else
        {
            OnPreferencsButtonComponent();
            OffAbandonButtonComponent();
        }
        _isOpenAbandonDirty = false;
    }
}

void PreferencesManager::SetGraphicsOptions(std::string_view option, bool enable)
{
    if ("SSR" == option)
        UmPreferences.SetSSR(enable);
    else if ("SSAO" == option)
        UmPreferences.SetSSAO(enable);
    else if ("Bloom" == option)
        UmPreferences.SetBloom(enable);
    else if ("VolumetricFog" == option)
        UmPreferences.SetVolumetricFog(enable);
}
void PreferencesManager::SetGraphicsQuality(PreferencesSystem::TextureQuality quality)
{
    UmPreferences.SetTextureQuality(quality);
}

void PreferencesManager::SetVolume(std::string_view option, int value)
{
    if ("MasterVolume" == option)
    {
        UmPreferences.SetMasterVolume((float)value, (float)MaxVolume);
    }
    else if ("BGMVolume" == option)
    {
        UmPreferences.SetBGMVolume((float)value, (float)MaxVolume);
    }
    else if ("SFXVolume" == option)
    {
        UmPreferences.SetSFXVolume((float)value, (float)MaxVolume);
    }
}

void PreferencesManager::OnPreferencesWindow(const Input::Controller&)
{
    _isOpen      = true;
    _isOpenDirty = true;
}

void PreferencesManager::OffPreferencesWindow()
{
    _isOpen      = false;
    _isOpenDirty = true;
}

void PreferencesManager::AddPreferencesButton(Component* comps) 
{
    _preferencesButtons.push_back(comps);
}

void PreferencesManager::AddAbandonButton(Component* comps)
{
    _abandonButtons.push_back(comps);
}

void PreferencesManager::OpenAbadonButtons()
{
    _isOpenAbandonDirty  = true;
    _isOpenAbandonButton = true;
}

void PreferencesManager::CloseAbandonButtons()
{
    _isOpenAbandonDirty  = true;
    _isOpenAbandonButton = false;
}

void PreferencesManager::OffAbandonButtonComponent() 
{
    for (auto& comp:_abandonButtons)
    {
        comp->Enable = false;
    }
}

void PreferencesManager::OffPreferencsButtonComponent() 
{
    for (auto& comp : _preferencesButtons)
    {
        comp->Enable = false;
    }
}

void PreferencesManager::OnAbandonButtonComponent() 
{
    for (auto& comp : _abandonButtons)
    {
        comp->Enable = true;
    }
}

void PreferencesManager::OnPreferencsButtonComponent() 
{
    for (auto& comp : _preferencesButtons)
    {
        comp->Enable = true;
    }
}
