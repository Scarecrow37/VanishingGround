#include "pchScripts.h"
#include "BackButton.h"
#include "Scripts/Preferences/PreferencesManager.h"

UMREAL_COMPONENT(BackButton)

BackButton::BackButton() = default;

BackButton::~BackButton() = default;

void BackButton::Awake()
{
    GameObject* manager = GameObject::Find("PreferencesManager").lock().get();
    if (manager)
    {
        _preferencesManager = manager->GetComponent<PreferencesManager>();
        if (nullptr == _preferencesManager)
            UmLogger.Log(LogLevel::LEVEL_WARNING, "Preferences manager not registered!");
        if (_preferencesManager)
            _preferencesManager->AddPreferencesButton(this);
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, "Preferences manager not registered!");
    }

}

void BackButton::Update()
{
    if (_dirtyFlag)
    {
        if (_preferencesManager)
            _preferencesManager->OffPreferencesWindow();
        _dirtyFlag = false;
    }
}

void BackButton::Reset() 
{
    BindInputAction(ControllerButton::B, Action::PRESSED, this, &BackButton::DirtyOffFlag);
}

void BackButton::DirtyOffFlag(const Input::Controller&) 
{
    _dirtyFlag = true;
    UmAudio.Play("-40030");
}
