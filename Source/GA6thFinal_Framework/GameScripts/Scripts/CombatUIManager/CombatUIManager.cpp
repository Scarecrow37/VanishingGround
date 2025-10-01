#include "pchScripts.h"
#include "CombatUIManager.h"
#include "Preferences/PreferencesManager.h"

UMREAL_COMPONENT(CombatUIManager)

CombatUIManager::CombatUIManager() = default;

CombatUIManager::~CombatUIManager() = default;

void CombatUIManager::Refresh()
{
    if (auto turnQueue = GameObject::FindWithTag("Turn Queue Panel").lock())
    {
        turnQueue->ActiveSelf = true;
    }

    if (auto HUD = GameObject::FindWithTag("Character HUD Group").lock())
    {
        HUD->ActiveSelf = true;
    }

    if (auto revelationPanel = GameObject::FindWithTag("Revelations Panel").lock())
    {
        revelationPanel->ActiveSelf = true;
    }

    if (auto weaponPanel = GameObject::FindWithTag("Weapon Panel").lock())
    {
        weaponPanel->ActiveSelf = true;
    }

    if (auto accessoriesPanel = GameObject::FindWithTag("Accessories Panel").lock())
    {
        accessoriesPanel->ActiveSelf = true;
    }
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
        // Initialization code here
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
