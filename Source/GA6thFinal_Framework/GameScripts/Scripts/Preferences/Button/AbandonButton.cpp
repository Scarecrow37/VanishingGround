#include "pchScripts.h"
#include "AbandonButton.h"
#include "Scripts/Preferences/PreferencesManager.h"

UMREAL_COMPONENT(AbandonButton)

AbandonButton::AbandonButton() = default;
AbandonButton::~AbandonButton() = default;

void AbandonButton::Awake() 
{
    int childCnt = transform->Parent->GetChildCount();
    for (int i = 0; i < childCnt; ++i)
    {
        Transform* child = transform->Parent->GetChild(i);
        if (nullptr == child)
            continue;
        if (child->gameObject->CompareTag("AbandonInfoPannel"))
            _abandonPannel = &(child->gameObject);
    }
    _abandonPannel->SetActive(false);

    GameObject* preferencesManager = GameObject::Find("PreferencesManager").lock().get();
    if (preferencesManager)
    {
        PreferencesManager* manager = preferencesManager->GetComponent<PreferencesManager>();
        manager->AddPreferencesButton(this);
    }
}

void AbandonButton::Update() 
{
    if (_dirtyFlag)
    {
        _abandonPannel->SetActive(true);
        GameObject* preferencesManager = GameObject::Find("PreferencesManager").lock().get();
        if (preferencesManager)
        {
            PreferencesManager* manager = preferencesManager->GetComponent<PreferencesManager>();
            manager->OpenAbadonButtons();
        }
        _dirtyFlag = false;
    }
}

void AbandonButton::Reset() 
{
    BindInputAction(ControllerButton::X, Action::PRESSED, this, &AbandonButton::DirtyOnFlag);
}

void AbandonButton::DirtyOnFlag(const Input::Controller&) 
{
    _dirtyFlag = true;
}
