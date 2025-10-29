#include "pchScripts.h"
#include "PlayerSystem.h"
#include "WeaponSystem/WeaponSystem.h"
#include "WeaponSystem/WeaponTable/WeaponTableComponent.h"
#include "RevelationSystem/RevelationSystem.h"
#include "ItemDropSystem/ItemDropSystem.h"
#include "Stats/Player/PlayerStatsComponent.h"
#include "AccessorySystem/AccessorySystem.h"
#include "ConsumableSystem/ConsumableSystem.h"
#include "UI/Elements/Image/ImageElement.h"
#include "Stats/Player/PlayerStats.h"

UMREAL_COMPONENT(PlayerSystem)

namespace
{
    template <typename T>
    bool CheckMessage(T* ptr)
    {
        bool        result = true;
        std::string message;
        message = typeid(T).name();
        if (nullptr == ptr)
        {
            message += (const char*)u8" 컴포넌트를 추가해주세요";
            result = false;
        }
        else
        {
            message += " ok";
            result = true;
        }
        ImGui::Text(message.c_str());
        return result;
    }

    template <typename T>
    bool CheckWithLog(T* ptr)
    {
        bool        result  = true;
        std::string message = (const char*)u8"플레이어 시스템에 ";
        message             = typeid(T).name();
        if (nullptr == ptr)
        {
            message += (const char*)u8" 컴포넌트를 추가해주세요";
            result = false;
            UmLogger.Log(LogLevel::LEVEL_WARNING, message);
        }
        else
        {
            message += " ok";
            result = true;
        }
        return result;
    }
} 

PlayerSystem::PlayerSystem() = default;
PlayerSystem::~PlayerSystem() = default;

void PlayerSystem::SetStatsGameStart() 
{
    if (_singletonObject.IsSingleTon())
    {
        if (_playerStatsComponent)
        {
            PlayerStats& stats           = _playerStatsComponent->GetStats();
            stats.CurrentHP              = stats.MaxHP;
            stats.CurrentChainCount      = 0;
            stats.CurrentChainRoundCount = stats.MaxChainRoundCount;
        }      
    }
}

void PlayerSystem::SetStatsCombatStart() 
{
    if (_singletonObject.IsSingleTon())
    {
        if (_playerStatsComponent)
        {
            PlayerStats& stats           = _playerStatsComponent->GetStats();
            stats.CurrentChainCount      = 0;
            stats.CurrentChainRoundCount = stats.MaxChainRoundCount;
        }        
    }
}

void PlayerSystem::NotifyPlayerHP() 
{
    if (true == CheckWithLog(_playerStatsComponent))
    {
        PlayerStats& stats = _playerStatsComponent->GetStats();
        int          hp    = stats.CurrentHP;
        stats.CurrentHP    = hp;
    }
}

void PlayerSystem::Reset() 
{
    _singletonObject.SetSingleTon();
    _singletonComponent.SetSingleTon();
    CheckSystem();
}

void PlayerSystem::Awake()
{
    if (_singletonObject.TrySingleTon(true))
    {
        _singletonComponent.TrySingleTon();
        CheckSystem();
        if (true == CheckWithLog(_playerStatsComponent))
        {
            PlayerStats& stats = _playerStatsComponent->GetStats();
            stats.RegisterHUD();
        }
    }
}

void PlayerSystem::Start() 
{
    if (ReflectFields->RevivePlayer)
    {
        SetStatsGameStart();
    }
}

void PlayerSystem::OnDestroy() 
{
    if (_singletonObject.IsSingleTon())
    {
        if (true == CheckWithLog(_playerStatsComponent))
        {
            PlayerStats& stats = _playerStatsComponent->GetStats();
            stats.UnregisterHUD();
        }
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
    CheckMessage(_consumableSystem);
    CheckMessage(_itemDropSystem);
    CheckMessage(_playerStatsComponent);
}

void PlayerSystem::CheckSystem() 
{
    _weaponSystem         = GetComponent<WeaponSystem>();
    _weaponTableComponent = GetComponent<WeaponTableComponent>();
    _revelationSystem     = GetComponent<RevelationSystem>();
    _accessorySystem      = GetComponent<AccessorySystem>();
    _consumableSystem     = GetComponent<ConsumableSystem>();
    _itemDropSystem       = GetComponent<ItemDropSystem>();
    _playerStatsComponent = GetComponent<PlayerStatsComponent>();
}


