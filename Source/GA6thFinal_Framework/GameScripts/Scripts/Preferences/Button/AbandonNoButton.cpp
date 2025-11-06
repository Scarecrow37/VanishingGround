#include "pchScripts.h"
#include "AbandonNoButton.h"
#include "Scripts/Preferences/Button/AbandonButton.h"
#include "Scripts/Preferences/PreferencesManager.h"

UMREAL_COMPONENT(AbandonNoButton)

AbandonNoButton::AbandonNoButton() = default;
AbandonNoButton::~AbandonNoButton() = default;

void AbandonNoButton::Awake() 
{
    _abandonPannel = GameObject::Find("AbandonPannel").lock().get();
    if (nullptr == _abandonPannel)
        UmLogger.Log(LogLevel::LEVEL_ERROR, "Abandon Pannel is not Created");

    GameObject* preferencesManager  = GameObject::Find("PreferencesManager").lock().get();
    if (preferencesManager)
    {
        PreferencesManager* manager = preferencesManager->GetComponent<PreferencesManager>();
        if (manager)
            manager->AddAbandonButton(this);
    }
}

void AbandonNoButton::Update() 
{
    if (_dirtyFlag)
    {
        if (_abandonPannel)
            _abandonPannel->SetActive(false);
        GameObject* preferencesManager = GameObject::Find("PreferencesManager").lock().get();
        if (preferencesManager)
        {
            PreferencesManager* manager = preferencesManager->GetComponent<PreferencesManager>();
            if (manager)
                manager->CloseAbandonButtons();
        }
        _dirtyFlag = false;
    }
}

void AbandonNoButton::Reset() 
{
    BindInputAction(ControllerButton::B, Action::PRESSED, this, &AbandonNoButton::DirtyOffFlag);
}

void AbandonNoButton::DirtyOffFlag(const Input::Controller&) 
{
    _dirtyFlag = true;
    UmAudio.Play("-901004");
}
