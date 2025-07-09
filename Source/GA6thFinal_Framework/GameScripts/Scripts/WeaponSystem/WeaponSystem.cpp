#include "pchScripts.h"
#include "WeaponSystem.h"
#include <Stats/WeaponTable/WeaponTableComponent.h>

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

void WeaponSystem::RoolRandomSpeed()
{
    for (auto& weapons : _equipWeapons)
    {
        weapons.RollRandomSpeed();
    }
}

WeaponStats WeaponSystem::EquipWeapon(int slot, const WeaponStats& weaponStats)
{
    WeaponStats originWeapon;
    if (0 <= slot && slot < _equipWeapons.size())
    {
        originWeapon = _equipWeapons[slot];
    }
    return originWeapon;
}

void WeaponSystem::SetCurrentWeaponSlot(int slot)
{
    slot               = std::clamp(slot, 0, (int)EQUIP_WEAPONS_SIZE - 1);
    _currentWeaponSlot = slot;
}

int WeaponSystem::GetRoundSpeedToSlot(int slot)
{
    int speed      = _equipWeapons[slot].Speed;
    int roundSpeed = _equipWeapons[slot].RandomSpeed;
    return speed + roundSpeed;
}

void WeaponSystem::ImguiEquipWeapons()
{
    if (ImGui::TreeNodeEx("Weapons", ImGuiTreeNodeFlags_DefaultOpen))
    {
        static const WeaponStats* changeWeaponSelect = nullptr;
        auto                      RightClickContext  = [&](int id) {
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
                                ImGui::PushStyleColor(ImGuiCol_Text,
                                                                            WeaponTableComponent::GetWeaponTypeColor(stats.Type));
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
                weapon             = *changeWeaponSelect;
                changeWeaponSelect = nullptr;
            }
            ImGui::PopID();
            ImGui::PopStyleColor(1);
        }
        ImGui::TreePop();
    }
}