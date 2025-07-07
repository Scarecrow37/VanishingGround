#include "pchScripts.h"
#include "WeaponTableComponent.h"
WeaponTableComponent::WeaponTableComponent() = default;
WeaponTableComponent::~WeaponTableComponent()
{
    if (static_instance == this)
    {
        static_instance = nullptr;
    }
}

bool WeaponTableComponent::RenameWeapon(WeaponStats& weapon, std::string_view newName)
{
    bool result = false;
    auto findIter = _weaponTable.find(newName.data());
    if (findIter == _weaponTable.end())
    {
        WeaponStats& newWeapon = _weaponTable[newName.data()];
        newWeapon.DeserializedReflectFields(weapon.SerializedReflectFields()); 
        newWeapon.SetName(newName); // 이름 변경

        std::string_view prevName = weapon.Name;
        _weaponTable.erase(prevName.data());    //기존 삭제
        result = true;
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, (const char*)u8"이미 존재하는 Weapon 이름 입니다.");
    }
    return result;
}

bool WeaponTableComponent::InsertWeapon(WeaponStats& weapon)
{
    bool result = false;
    std::string_view name = weapon.Name;
    auto             findIter = _weaponTable.find(name.data());
    if (findIter == _weaponTable.end())
    {
        WeaponStats& newWeapon = _weaponTable[name.data()];
        newWeapon.DeserializedReflectFields(weapon.SerializedReflectFields());
        result = true;
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, (const char*)u8"이미 존재하는 Weapon 이름 입니다.");
    }
    return result;
}

bool WeaponTableComponent::EraseWeapon(WeaponStats& weapon)
{
    bool             result   = false;
    std::string_view name     = weapon.Name;
    auto             findIter = _weaponTable.find(name.data());
    if (findIter != _weaponTable.end())
    {
        _weaponTable.erase(name.data());
        result = true;
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, (const char*)u8"존재 하지 않는 Weapon 이름 입니다.");
    }
    return result;
}

void WeaponTableComponent::Awake()
{
    if (nullptr == static_instance)
    {
        static_instance = this;
        gameObject->AddTag(TAG);
    }
    else
    {
        GameObject::Destroy(this);
    }
}

void WeaponTableComponent::ImGuiDrawPropertysEvent()
{
    if (ImGui::TreeNodeEx("Weapon Table", ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (auto& [key, weapon] : _weaponTable)
        {
            auto RightClickContext = [&]() 
            {
                if (ImGui::BeginPopupContextItem())
                {
                    if (ImGui::MenuItem("Rename"))
                    {
                        _imguiEvent.RenameBuffer    = key;
                        _imguiEvent.SelectWeapon    = &weapon;
                        _imguiEvent.OpenRenamePopup = true;
                    }
                    if (ImGui::MenuItem("Delete"))
                    {
                        _imguiEvent.DeleteTableBuffer = key;      
                        _imguiEvent.OpenDeletePopup   = true;
                    }
                    ImGui::EndPopup();
                }
            };

            auto GetWeaponTypeColor = [&](WeaponStats::WeaponType type) 
            {
                constexpr std::array<ImVec4, 3> typeColorTable;
                switch (type)
                {
                case WeaponStats::WeaponType::SWORD:
                    return ImVec4(1.0f, 0.7f, 0.2f, 1.0f); // 밝은 황금빛 오렌지
                case WeaponStats::WeaponType::DAGGER:
                    return ImVec4(0.8f, 0.5f, 0.2f, 1.0f); // 견고한 갈색
                case WeaponStats::WeaponType::WARHAMMER:
                    return ImVec4(0.2f, 0.9f, 0.9f, 1.0f); // 선명한 시안
                }
                return ImVec4(0.6f, 0.6f, 0.6f, 1.0f); // 기본 회색 (다른 타입 또는 알 수 없는 타입)
            };

            int itemID = 0;
            ImGui::PushStyleColor(ImGuiCol_Text, GetWeaponTypeColor(weapon.Type));
            if (ImGui::TreeNodeEx(key.data(), ImGuiTreeNodeFlags_OpenOnArrow))
            {
                RightClickContext(); // TreeNode가 열렸을 때도 컨텍스트 메뉴가 동작하도록 안으로 이동
                if (ImGui::BeginTable(key.data(), 1, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::PushID(itemID++);
                    weapon.ImGuiDrawPropertys();
                    ImGui::PopID(); // 고유 ID 끝
                    ImGui::EndTable();
                } 
                ImGui::TreePop();
            }
            else
            {
                RightClickContext();
            }
            ImGui::PopStyleColor(1);
        }
        ImGui::TreePop();
    }

    if (_imguiEvent.OpenDeletePopup)
    {
        _imguiEvent.OpenDeletePopup = false;
        ImGui::OpenPopup("Weapon Table Delete Modal Popup");
    }

    if (ImGui::BeginPopupModal("Weapon Table Delete Modal Popup"))
    {
        ImGui::Text((const char*)u8"이 작업은 되돌릴 수 없습니다.");
        ImGui::Text("%s", _imguiEvent.DeleteTableBuffer.c_str());
        ImGui::SameLine();
        ImGui::Text((const char*)u8"정말 삭제하시겠습니까?");
        ImGui::Separator();
        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            _imguiEvent.DeleteTableBuffer = STR_NULL;
            EraseWeapon(_weaponTable[_imguiEvent.DeleteTableBuffer]);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            _imguiEvent.DeleteTableBuffer = STR_NULL;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (_imguiEvent.OpenRenamePopup)
    {
        _imguiEvent.OpenRenamePopup = false;
        ImGui::OpenPopup("Weapon Table Rename Popup");
    }

    if (ImGui::BeginPopup("Weapon Table Rename Popup"))
    {
        ImGui::InputText("##Rename", &_imguiEvent.RenameBuffer);
        ImGui::SameLine();
        if (ImGui::Button("Rename"))
        {
            if (false == _imguiEvent.RenameBuffer.empty())
            {
                RenameWeapon(*_imguiEvent.SelectWeapon, _imguiEvent.RenameBuffer);
                _imguiEvent.SelectWeapon = nullptr;
                _imguiEvent.RenameBuffer = STR_NULL;
                ImGui::CloseCurrentPopup();
            }
        }
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            _imguiEvent.SelectWeapon = nullptr;
            _imguiEvent.RenameBuffer = STR_NULL;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    static std::string newWeaponName;
    ImGui::InputText("##New Weapon", &newWeaponName);
    ImGui::SameLine();
    if (ImGui::Button("New Weapon"))
    {
        if (false == newWeaponName.empty())
        {
            WeaponStats stats;
            stats.SetName(newWeaponName);
            bool result = InsertWeapon(stats);
            if (true == result)
            {
                newWeaponName.clear();
            }
        }
    }
}

void WeaponTableComponent::SerializedReflectEvent() 
{
    ReflectFields->_tableDatas.clear();
    for (auto& [key, weapon] : _weaponTable)
    {
        ReflectFields->_tableDatas.push_back(weapon.SerializedReflectFields());
    }
}

void WeaponTableComponent::DeserializedReflectEvent() 
{
    for (auto& weapon : ReflectFields->_tableDatas)
    {
        WeaponStats stats;
        stats.DeserializedReflectFields(weapon);
        InsertWeapon(stats);
    }
}
