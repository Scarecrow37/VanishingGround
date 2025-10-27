#include "pchScripts.h"
#include "WeaponSystem.h"
#include <WeaponSystem/WeaponTable/WeaponTableComponent.h>
#include <TurnSystem/TurnMode/TurnMode.h>


UMREAL_COMPONENT(WeaponSystem)

WeaponSystem::WeaponSystem()
{

}
WeaponSystem::~WeaponSystem()
{
    
}

void WeaponSystem::Reset() 
{
    _singletonComponent.SetSingleTon();
}

void WeaponSystem::Awake() 
{
    _singletonComponent.TrySingleTon();
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
    TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance();
    auto isPlay = UmCore->IsPlay();
    if (isPlay && turnMode)
    {
        WeaponElement& curr = _equipWeapons[_currentWeaponSlot];
        WeaponElement& next = _equipWeapons[slot];
        if (false == curr._actions.empty())
        {
            for (auto& action : curr._actions)
            {
                if (action)
                {
                    action->SetDestroy();
                }             
            }      
        }
        if (false == next._actions.empty())
        {
            for (auto& action : next._actions)
            {
                if (action)
                {
                    turnMode->AddTurnAction(action.get());
                    std::string msg = next.Stats.WeaponName;
                    msg += (const char*)u8" 효과 발동";
                    action->OnActionActive = [msg]() { UmLogger.Message(LogLevel::LEVEL_DEBUG, msg); };
                }
            }      
        }       
    }
    if (-1 != _lastWeaponSlot)
        _lastWeaponSlot = slot; // 처음에는 마지막 무기와 최초 무기가 같다.
    else 
        _lastWeaponSlot = _currentWeaponSlot;

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
                WeaponTableComponent* weaponTableComponent = SingletonComponent<WeaponTableComponent>::GetInstance();
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
                    WeaponTableComponent* weaponTable = SingletonComponent<WeaponTableComponent>::GetInstance();
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

        auto ValidWeaponInfo = [](const std::string& weaponName) 
        {
            WeaponTableComponent* weaponTableComponent = SingletonComponent<WeaponTableComponent>::GetInstance();
            if (weaponTableComponent)
            {
                const WeaponElement* element = weaponTableComponent->GetWeaponToName(weaponName);
                if (nullptr == element)
                {
                    ImGui::SameLine();
                    ImGui::Text("(!)");
                    ImGuiHelper::HoveredToolTip(u8"테이블에 존재하지 않는 무기입니다.");
                }
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
                ValidWeaponInfo(weaponName);
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
                ValidWeaponInfo(weaponName);
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
                WeaponTableComponent* weaponTableComponent = SingletonComponent<WeaponTableComponent>::GetInstance();
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