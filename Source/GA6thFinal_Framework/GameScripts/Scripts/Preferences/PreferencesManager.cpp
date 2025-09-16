#include "pchScripts.h"
#include "PreferencesManager.h"
#include "PrefrencesWindow.h"
PreferencesManager::PreferencesManager() = default;
PreferencesManager::~PreferencesManager() = default;

void PreferencesManager::Reset()
{
    BindInputAction(ControllerButton::BACK, Action::PRESSED, this, &PreferencesManager::OnPreferencesWindow);
}

void PreferencesManager::Awake()
{
    auto pannel = GameObject::Find("PreferencesPannel");
    pannel.lock()->SetActive(false);
}

void PreferencesManager::Update()
{
}

void PreferencesManager::OnPreferencesWindow(const Input::Controller&) 
{
    auto pannel = GameObject::Find("PreferencesPannel");
    pannel.lock()->SetActive(true);
}
