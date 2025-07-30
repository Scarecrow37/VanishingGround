#include "pchScripts.h"
#include "WeaponSystem.h"
#include <WeaponSystem/WeaponTable/WeaponTableComponent.h>
#include <TurnSystem/TurnMode/TurnMode.h>

WeaponSystem::WeaponSystem() = default;
WeaponSystem::~WeaponSystem()
{
    if (this == static_instance)
    {
        static_instance = nullptr;
    }
}

void WeaponSystem::Reset() 
{
    static_instance = this;
}

void WeaponSystem::Awake() 
{

}

void WeaponSystem::SerializedReflectEvent()
{
    for (size_t i = 0; i < EQUIP_WEAPONS_SIZE; ++i)
    {
        ReflectFields->EquipWeaponsData[i] = _equipWeapons[i].SerializedReflectFields();
    }
}

void WeaponSystem::DeserializedReflectEvent() 
{
    for (size_t i = 0; i < EQUIP_WEAPONS_SIZE; ++i)
    {
        _equipWeapons[i].DeserializedReflectFields(ReflectFields->EquipWeaponsData[i]);
    }
}

void WeaponSystem::ImGuiDrawPropertysEvent() 
{
    ImguiEquipWeapons();
}

void WeaponSystem::RollRandomSpeed()
{
    for (auto& weapons : _equipWeapons)
    {
        weapons.Stats.RollRandomSpeed();
    }
}

WeaponElement WeaponSystem::EquipWeapon(int slot, const WeaponElement& weapon)
{
    WeaponElement originWeapon;
    if (0 <= slot && slot < _equipWeapons.size())
    {
        originWeapon = _equipWeapons[slot];
        _equipWeapons[slot] = weapon;
    }
    return originWeapon;
}

void WeaponSystem::SetCurrentWeaponSlot(int slot)
{
    if (slot == _currentWeaponSlot)
    {
        return;
    }

    if (slot < 0 || slot >= _equipWeapons.size())
    {
        UmLogger.Log(LogLevel::LEVEL_DEBUG, "out of index");
        return;
    }
    TurnMode* turnMode = TurnMode::GetInstance();
    auto isPlay = Global::IsPlay();
    if (isPlay && turnMode)
    {
        WeaponElement& curr = _equipWeapons[_currentWeaponSlot];
        WeaponElement& next = _equipWeapons[slot];
        if (curr._action)
        {
            curr._action->SetDestroy();
        }
        if (next._action)
        {
            turnMode->AddTurnAction(next._action.get());
            std::string msg = next.Stats.WeaponName;
            msg += (const char*)u8" 효과 발동";
            next._action->OnActionActive = [msg]() { UmLogger.Message(LogLevel::LEVEL_DEBUG, msg); };
        }       
    }
    _currentWeaponSlot = slot;
}

int WeaponSystem::GetRoundSpeedToSlot(int slot)
{
    int speed      = _equipWeapons[slot].Stats.Speed;
    int roundSpeed = _equipWeapons[slot].Stats.RandomSpeed;
    return speed + roundSpeed;
}

void WeaponSystem::ImguiEquipWeapons()
{
    auto WeaponsRightClickContext = [&]() 
    {
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Reset All"))
            {
                WeaponTableComponent* weaponTableComponent = WeaponTableComponent::GetInstance();
                if (weaponTableComponent)
                {
                    auto& weaponTable = weaponTableComponent->GetWeaponTable();
                    for (auto& weapon : _equipWeapons)
                    {
                        const std::string& weaponName = weapon.Stats.WeaponName;
                        auto findIter = weaponTable.find(weaponName);
                        if (findIter != weaponTable.end())
                        {
                            weapon = findIter->second;
                        }               
                    }
                }
            }
            ImGuiHelper::HoveredToolTip(u8"플레이어의 무기 스텟들을 무기 테이블 기준으로 초기화 시킵니다.");
            ImGui::EndPopup();
        }
    };

    if (ImGui::TreeNodeEx("Weapons", ImGuiTreeNodeFlags_DefaultOpen))
    {
        WeaponsRightClickContext();
        static const WeaponElement* changeWeaponSelect = nullptr;
        static bool                 resetWeponSelect   = false;

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
                        for (auto& [name, element] : weaponTable->GetWeaponTable())
                        {
                            if (filter.PassFilter(name.c_str()))
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, WeaponStats::GetGradeToColor(element.Stats.Grade));
                                if (ImGui::Selectable(name.c_str()))
                                {
                                    changeWeaponSelect = &element;
                                }
                                ImGui::PopStyleColor(1);
                            }
                        }
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::MenuItem("Reset"))
                {
                    resetWeponSelect = true;
                }
                ImGui::EndPopup();
            }
        };

        int itemID = 0;
        for (auto& weapon : _equipWeapons)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, WeaponStats::GetGradeToColor(weapon.Stats.Grade));
            ImGui::PushID(itemID++);
            const std::string& weaponName = weapon.Stats.WeaponName;
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
            
            // 무기 변경
            if (nullptr != changeWeaponSelect)
            {
                weapon             = *changeWeaponSelect;
                changeWeaponSelect = nullptr;
            }

            //무기 스텟 테이블 정보로 리셋
            if (true == resetWeponSelect)
            {
                WeaponTableComponent* weaponTableComponent = WeaponTableComponent::GetInstance();
                if (weaponTableComponent)
                {
                    auto& weaponTable = weaponTableComponent->GetWeaponTable();
                    auto findIter = weaponTable.find(weaponName);
                    if (findIter != weaponTable.end())
                    {
                        weapon = findIter->second;
                        resetWeponSelect = false;
                    }
                }
            }

            ImGui::PopID();
            ImGui::PopStyleColor(1);
        }
        ImGui::TreePop();
    }
    else
    {
        WeaponsRightClickContext();
    }
}