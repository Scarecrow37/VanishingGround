#include "pchScripts.h"
#include "CombatUIManager.h"
#include "Preferences/PreferencesManager.h"

UMREAL_COMPONENT(CombatUIManager)

CombatUIManager::CombatUIManager() = default;

CombatUIManager::~CombatUIManager() = default;

void CombatUIManager::Refresh()
{
    _charactorHUDGroup.FindUI();
}

void CombatUIManager::SetActiveUI(bool active) 
{
    // TODO: 전투 UI 활성화/비활성화 구현
}

void CombatUIManager::Reset()
{
    _singletonObject.SetSingleTon();
    _singletonComponent.SetSingleTon();
}

void CombatUIManager::Awake() 
{
    BindInputAction(ControllerButton::BACK, Action::PRESSED, this,
                    &CombatUIManager::PreferencesKeyDown);  // 옵션 창 키 바인딩

    if (_singletonObject.TrySingleTon(true) &&
        _singletonComponent.TrySingleTon())
    {
        Refresh();
    }
}

void CombatUIManager::ImGuiDrawPropertysEvent() 
{
}

void CombatUIManager::SerializedReflectEvent() 
{
}

void CombatUIManager::DeserializedReflectEvent() 
{
}

void CombatUIManager::PreferencesKeyDown(const Input::Controller&) 
{
    //TODO: 마지막 포커스된 UI를 전달해야함
    if (PreferencesManager* manager = SingletonComponent<PreferencesManager>::GetInstance())
    {
        manager->OnPreferencesWindow(nullptr);
    }
}
