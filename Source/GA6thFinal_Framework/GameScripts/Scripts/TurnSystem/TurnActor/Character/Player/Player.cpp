#include "pchScripts.h"
#include "Player.h"
#include "Stats/Player/PlayerStats.h"
#include "Stats/Player/PlayerStatsComponent.h"
#include "GameCore/FSM/FiniteStateMachine.h"
#include <Stats/WeaponTable/WeaponTableComponent.h>

//Condition
#include "Condition/PlayerStartCondition.h"
#include "Condition/PlayerExitCondition.h"
#include "Condition/PlayerDeadCondition.h"

//State
#include "State/PlayerWaitTurnState.h"
#include "State/PlayerPlayTurnState.h"
#include "State/PlayerDeadState.h"

Player::Player()
{
   
}
Player::~Player() = default;

void Player::Awake() 
{
    Base::Awake();
    gameObject->AddTag(TAG);
    BuildPlayerFSM();

    if (nullptr == GetPlayerStats())
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, (const char*)u8"Player Stats를 추가해주세요");
    }
}

void Player::Update() 
{

}

void Player::SerializedReflectEvent() 
{
    for (size_t i = 0; i < EQUIP_WEAPONS_SIZE; ++i)
    {
        ReflectFields->EquipWeaponsData[i] = _equipWeapons[i].SerializedReflectFields();
    }
}

void Player::DeserializedReflectEvent() 
{
    for (size_t i = 0; i < EQUIP_WEAPONS_SIZE; ++i)
    {
        _equipWeapons[i].DeserializedReflectFields(ReflectFields->EquipWeaponsData[i]);
    }
}

int Player::GetSpeed()
{
    return _equipWeapons[_currentWeaponSlot].Speed;
}

int Player::GetRandomSpeed()
{
    return _equipWeapons[_currentWeaponSlot].RandomSpeed;
}

void Player::PlayTurn()
{
    Base::PlayTurn();
    std::string_view weaponName = GetCurrentWeaponStats().Name;
    std::string      message    = std::format("{}{}{}", (const char*)u8"Player 턴 시작. ", "Weapon : ", weaponName);
    UmLogger.Message(LogLevel::LEVEL_TRACE, message);
}

void Player::EndTurn()
{
    Base::EndTurn();
    UmLogger.Message(LogLevel::LEVEL_TRACE, (const char*)u8"Player 턴 종료.");
}

void Player::Dead()
{
    Base::Dead();
    UmLogger.Message(LogLevel::LEVEL_DEBUG, (const char*)u8"플레이어 사망!!!");
}


void Player::ImGuiDrawPropertysEvent()
{
    ImguiEquipWeapons();
}

CharacterStats* Player::GetCharacterStats()
{
    CharacterStats* stats = nullptr;
    PlayerStatsComponent* playerStatsComponent = GetPlayerStats();
    if (nullptr != playerStatsComponent)
    {
        stats = playerStatsComponent->GetStats();
    }
    return stats;
}

void Player::OnRoundStart() 
{
    Base::OnRoundStart();
    for (auto& weapons : _equipWeapons)
    {
        weapons.RollRandomSpeed();
    }
}

WeaponStats Player::EquipWeapon(int slot, const WeaponStats& weaponStats)
{
    WeaponStats originWeapon;
    if (0 <= slot && slot < _equipWeapons.size())
    {
        originWeapon = _equipWeapons[slot];
    }
    return originWeapon;
}

void Player::SetCurrentWeaponSlot(int slot)
{
    slot = std::clamp(slot, 0, (int)EQUIP_WEAPONS_SIZE);
    _currentWeaponSlot = slot;
}

int Player::GetRoundSpeedToSlot(int slot)
{
    int speed = _equipWeapons[slot].Speed;
    int roundSpeed = _equipWeapons[slot].RandomSpeed;
    return speed + roundSpeed;
}

void Player::ImguiEquipWeapons() 
{
    if (ImGui::TreeNodeEx("Weapons", ImGuiTreeNodeFlags_DefaultOpen))
    {
        static const WeaponStats* changeWeaponSelect = nullptr;
        auto RightClickContext = [&](int id) 
        {
            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::BeginMenu("Change"))
                {
                    WeaponTableComponent* weaponTable = WeaponTableComponent::GetInstance();
                    if (weaponTable)
                    {
                        static ImGuiTextFilter filter;
                        filter.Draw("filter");
                        for (auto& [name, stats] : weaponTable->GetWeaponTable())
                        {
                            if (filter.PassFilter(name.c_str()))
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, WeaponTableComponent::GetWeaponTypeColor(stats.Type));
                                if (ImGui::Selectable(name.c_str()))
                                {
                                    changeWeaponSelect = &stats;
                                }
                                ImGui::PopStyleColor(1);
                            }
                        }
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndPopup();
            }
        };

        int itemID = 0;
        for (auto& weapon : _equipWeapons)
        {        
            ImGui::PushStyleColor(ImGuiCol_Text, WeaponTableComponent::GetWeaponTypeColor(weapon.Type));
            ImGui::PushID(itemID++);
            std::string_view weaponName = weapon.Name;
            if (ImGui::TreeNodeEx(weaponName.data(), ImGuiTreeNodeFlags_OpenOnArrow))
            {
                RightClickContext(itemID);
                if (ImGui::BeginTable(weaponName.data(), 1, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    weapon.ImGuiDrawPropertys();
                    ImGui::EndTable();
                }
                ImGui::TreePop();
            }
            else
            {
                RightClickContext(itemID);
            }
            if (nullptr != changeWeaponSelect)
            {
                weapon = *changeWeaponSelect;
                changeWeaponSelect = nullptr;
            }
            ImGui::PopID(); 
            ImGui::PopStyleColor(1);
        }
        ImGui::TreePop();
    }
}

PlayerStatsComponent* Player::GetPlayerStats()
{
    if (nullptr == _playerStats)
    {
        _playerStats = GetComponent<PlayerStatsComponent>();
    }  
    return _playerStats;
}

int Player::GetShield()
{
    int shield = 0;
    PlayerStatsComponent* playerStats = GetPlayerStats();
    if (playerStats)
    {
        shield = playerStats->GetStats()->Shield;
    }
    return shield;
}

void Player::BuildPlayerFSM() 
{
    _finiteStateMachine = GetComponent<FiniteStateMachine>();
    if (nullptr == _finiteStateMachine)
    {
        _finiteStateMachine = &AddComponent<FiniteStateMachine>();

        //Conditions
        _finiteStateMachine->AddCondition<PlayerStartCondition>();
        _finiteStateMachine->AddCondition<PlayerExitCondition>();
        _finiteStateMachine->AddCondition<PlayerDeadCondition>();

        //States
        _fsmStates.PlayerWaitTurnState = _finiteStateMachine->AddState<PlayerWaitTurnState>();
        _fsmStates.PlayerPlayTurnState = _finiteStateMachine->AddState<PlayerPlayTurnState>();
        _fsmStates.PlayerDeadState     = _finiteStateMachine->AddState<PlayerDeadState>();

        //Transition
        _finiteStateMachine->AddTransition<PlayerWaitTurnState, PlayerStartCondition, PlayerPlayTurnState>();
        _finiteStateMachine->AddTransition<PlayerPlayTurnState, PlayerExitCondition, PlayerWaitTurnState>();

        _finiteStateMachine->AddTransition<PlayerDeadCondition, PlayerDeadState>();
        _finiteStateMachine->AddTransition<PlayerDeadState, PlayerExitCondition, PlayerWaitTurnState>();

        //Entry
        _finiteStateMachine->SetEntryState<PlayerWaitTurnState>();
    }
}
