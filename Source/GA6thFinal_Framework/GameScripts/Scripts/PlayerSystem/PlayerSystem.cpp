#include "pchScripts.h"
#include "PlayerSystem.h"
#include "WeaponSystem/WeaponSystem.h"
#include "WeaponSystem/WeaponTable/WeaponTableComponent.h"
#include "RevelationSystem/RevelationSystem.h"
#include "ItemDropSystem/ItemDropSystem.h"
#include "Stats/Player/PlayerStatsComponent.h"
#include "AccessorySystem/AccessorySystem.h"

PlayerSystem::PlayerSystem() = default;
PlayerSystem::~PlayerSystem() = default;

void PlayerSystem::Reset() 
{
    _singletonObject.SetSingleTon();
    CheckSystem();
}

void PlayerSystem::Awake()
{
    if (_singletonObject.TrySingleTon(true))
    {
       
    }
}

namespace
{
    template<typename T>
    void CheckMessage(T* ptr)
    {
        std::string message;
        message = typeid(T).name();
        if (nullptr == ptr)
        {
            message += (const char*)u8" 컴포넌트를 추가해주세요";
        }
        else
        {
            message += " ok";
        }
        ImGui::Text(message.c_str());
    }
}

void PlayerSystem::ImGuiDrawPropertysEvent() 
{
    if (ImGui::Button("Check System"))
    {
        CheckSystem();
    }
    CheckMessage(_weaponSystem);
    CheckMessage(_weaponTableComponent);
    CheckMessage(_revelationSystem);
    CheckMessage(_accessorySystem);
    CheckMessage(_itemDropSystem);
    CheckMessage(_playerStatsComponent);
}

void PlayerSystem::CheckSystem() 
{
    _weaponSystem         = GetComponent<WeaponSystem>();
    _weaponTableComponent = GetComponent<WeaponTableComponent>();
    _revelationSystem     = GetComponent<RevelationSystem>();
    _accessorySystem      = GetComponent<AccessorySystem>();
    _itemDropSystem       = GetComponent<ItemDropSystem>();
    _playerStatsComponent = GetComponent<PlayerStatsComponent>();
}


