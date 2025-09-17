#include "pchScripts.h"
#include "PreferencesManager.h"
#include "PrefrencesWindow.h"

UMREAL_COMPONENT(PreferencesManager)


PreferencesManager::PreferencesManager() = default;
PreferencesManager::~PreferencesManager() = default;

void PreferencesManager::Reset()
{
    BindInputAction(ControllerButton::BACK, Action::PRESSED, this, &PreferencesManager::OnPreferencesWindow);
    BindInputAction(ControllerButton::START, Action::PRESSED, this, &PreferencesManager::OffPreferencesWindow);
}

void PreferencesManager::Awake()
{
    auto pannel = GameObject::Find("PreferencesPannel");
    if (nullptr == pannel.lock())
        UmLogger.Log(LogLevel::LEVEL_ERROR, "환경설정 패널이 없습니다!");
    pannel.lock()->SetActive(false);
}

void PreferencesManager::Update()
{
}

void PreferencesManager::OnPreferencesWindow(const Input::Controller&) 
{
    auto pannel = GameObject::Find("PreferencesPannel");
    if (nullptr == pannel.lock())
        UmLogger.Log(LogLevel::LEVEL_ERROR, "환경설정 패널이 없습니다!");
    pannel.lock()->SetActive(true);
}

void PreferencesManager::OffPreferencesWindow(const Input::Controller&)
{
    auto pannel = GameObject::Find("PreferencesPannel");
    if (nullptr == pannel.lock())
        UmLogger.Log(LogLevel::LEVEL_ERROR, "환경설정 패널이 없습니다!");
    pannel.lock()->SetActive(false);
}
