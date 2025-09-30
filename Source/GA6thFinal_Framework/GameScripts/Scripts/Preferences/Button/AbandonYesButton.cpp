#include "pchScripts.h"
#include "AbandonYesButton.h"
#include "Scripts/Preferences/PreferencesManager.h"

UMREAL_COMPONENT(AbandonYesButton)

AbandonYesButton::AbandonYesButton() = default;
AbandonYesButton::~AbandonYesButton() = default;

void AbandonYesButton::Awake() 
{
    GameObject* preferencesManager = GameObject::Find("PreferencesManager").lock().get();
    if (preferencesManager)
    {
        PreferencesManager* manager = preferencesManager->GetComponent<PreferencesManager>();
        if (manager)
            manager->AddAbandonButton(this);
    }
}

void AbandonYesButton::Update()
{
    if (_dirtyFlag)
    {
        GameObject* preferencesManager = GameObject::Find("PreferencesManager").lock().get();
        if (preferencesManager)
        {
            PreferencesManager* manager = preferencesManager->GetComponent<PreferencesManager>();
            _dirtyFlag = false;
            if (manager)
            {
                manager->CloseAbandonButtons();
                manager->GoToMainMenu();
            }
        }
        else
        {
            _dirtyFlag = false;
        }
    }
}

void AbandonYesButton::Reset() 
{
    BindInputAction(ControllerButton::A, Action::PRESSED, this, &AbandonYesButton::DirtyOffFlag);
}

void AbandonYesButton::DirtyOffFlag(const Input::Controller&)
{
    _dirtyFlag = true;
    UmAudio.Play("-40020");
}