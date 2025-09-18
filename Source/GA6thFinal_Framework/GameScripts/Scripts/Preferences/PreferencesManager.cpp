#include "pchScripts.h"
#include "PreferencesManager.h"
#include "PrefrencesWindow.h"

UMREAL_COMPONENT(PreferencesManager)

PreferencesManager::PreferencesManager()  = default;
PreferencesManager::~PreferencesManager() = default;

void PreferencesManager::Reset()
{
    BindInputAction(ControllerButton::BACK, Action::PRESSED, this, &PreferencesManager::OnPreferencesWindow);
    BindInputAction(ControllerButton::START, Action::PRESSED, this, &PreferencesManager::OffPreferencesWindow);
}

void PreferencesManager::Awake()
{
    _preferencesPannel = GameObject::Find("PreferencesPannel").lock().get();
    if (nullptr == _preferencesPannel)
        UmLogger.Log(LogLevel::LEVEL_ERROR, "환경설정 패널이 없습니다!");
    else if (nullptr != _preferencesPannel)
        _preferencesPannel->SetActive(false);
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

void PreferencesManager::OnPreferencesWindow(const Input::Controller&)
{
    if (nullptr == _preferencesPannel)
        UmLogger.Log(LogLevel::LEVEL_ERROR, "환경설정 패널이 없습니다!");
    else if (nullptr != _preferencesPannel)
        _preferencesPannel->SetActive(true);
}

void PreferencesManager::OffPreferencesWindow(const Input::Controller&)
{
    if (nullptr == _preferencesPannel)
        UmLogger.Log(LogLevel::LEVEL_ERROR, "환경설정 패널이 없습니다!");
    else if (nullptr != _preferencesPannel)
        _preferencesPannel->SetActive(false);
}
