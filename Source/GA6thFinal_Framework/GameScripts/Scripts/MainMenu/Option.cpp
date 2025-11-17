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
        if (_preferencesManager)
        {
            _preferencesManager->OnPreferencesWindow(this);
        }
    }

    Debugger()([this] {
        // 아래는 디버그용 코드입니다.
        if (ImGui::Button("Preferences"))
        {
            Submit();
        }
    });
}

void Option::Submit()
{
    Base::Submit();
    _onDirtyFlag = true;
}