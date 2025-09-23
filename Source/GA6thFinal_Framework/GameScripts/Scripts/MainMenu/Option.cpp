#include "pchScripts.h"
#include "Option.h"
#include "Scripts/Preferences/PreferencesManager.h"

UMREAL_COMPONENT(Option)

Option::Option() = default;
Option::~Option() = default;

void Option::Awake() 
{
    __super::Awake();
    GameObject* manager = GameObject::Find("PreferencesManager").lock().get();
    if (manager)
    {
        _preferencesManager = manager->GetComponent<PreferencesManager>();
    }
}

void Option::Update()
{
    if (_onDirtyFlag)
    {
        _onDirtyFlag = false;
        _preferencesManager->OnPreferencesWindow();
    }
}

void Option::Submit()
{
    _onDirtyFlag = true;
}