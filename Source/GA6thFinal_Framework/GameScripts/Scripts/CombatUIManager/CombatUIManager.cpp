#include "pchScripts.h"
#include "CombatUIManager.h"
#include "Preferences/PreferencesManager.h"

UMREAL_COMPONENT(CombatUIManager)

CombatUIManager::CombatUIManager() = default;

CombatUIManager::~CombatUIManager() = default;

void CombatUIManager::Refresh()
{
    _charactorHUDGroup.FindUI();
    _charactorHUDGroup.RefreshEnemiesPosition();
}

void CombatUIManager::SetActiveUI(bool active) 
{
    _charactorHUDGroup.ActiveUI(active);
    _turnQueueGroup.ActiveUI(active);
    _consumableGroup.ActiveUI(active);
    _revelationsGroup.ActiveUI(active);
    _weaponGroup.ActiveUI(active);
    _accessoriesGroup.ActiveUI(active);
}

void CombatUIManager::Reset()
{
    _singletonComponent.SetSingleTon();
}

void CombatUIManager::Awake() 
{
    BindInputAction(ControllerButton::BACK, Action::PRESSED, this,
                    &CombatUIManager::PreferencesKeyDown);  // 옵션 창 키 바인딩

    if (_singletonComponent.TrySingleTon())
    {
        Refresh();
    }
}

void CombatUIManager::Update() 
{
    if (_charactorHUDGroup.IsValid())
    {
        _charactorHUDGroup.RefreshUIPosition();
    }
}

void CombatUIManager::FixedUpdate() 
{
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
