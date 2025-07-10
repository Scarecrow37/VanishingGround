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

const WeaponStats* WeaponTableComponent::GetWeaponToName(std::string_view name)
{
    WeaponStats* result = nullptr;
    auto findIter = _weaponTable.find(name.data());
    if (findIter != _weaponTable.end())
    {
        result = &findIter->second;
    }
    return result;
}

std::string WeaponTableComponent::SaveWeaponTable()
{
    ReflectFields->_tableDatas.clear();
    for (auto& [key, weapon] : _weaponTable)
    {
        ReflectFields->_tableDatas.push_back(weapon.SerializedReflectFields());
    }
    return rfl::json::write(ReflectFields->_tableDatas);
}

bool WeaponTableComponent::LoadWeaponTable(std::string_view data) 
{
    auto result = rfl::json::read<std::vector<std::string>>(data.data());
    if (result)
    {
        ReflectFields->_tableDatas = result.value();
        _weaponTable.clear();
        for (auto& weapon : ReflectFields->_tableDatas)
        {
            WeaponStats stats;
            stats.DeserializedReflectFields(weapon);
            InsertWeapon(stats);
        }
    } 
    return result;
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

void WeaponTableComponent::Reset() 
{
    static_instance = this;
    gameObject->AddTag(TAG);
}

void WeaponTableComponent::Awake()
{

}

void WeaponTableComponent::ImGuiDrawPropertysEvent()
{
    if (ImGui::Button("Edit Table "))
    {
        _imguiEvent.ShowTableEditor = true;
    }
   

    if (_imguiEvent.ShowTableEditor)
    {
        ImGuiViewport* viewPort = ImGui::GetMainViewport();
        ImVec2         center   = viewPort->GetCenter();
        ImVec2         size     = viewPort->Size;
        ImGui::SetNextWindowPos(center, ImGuiCond_Once, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(size, ImGuiCond_FirstUseEver);
        ImGui::Begin("Weapon Table Editor##E05D7DDE-9B06-40B2-A6CC-B7FB0632FD33", &_imguiEvent.ShowTableEditor, ImGuiWindowFlags_MenuBar);
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::MenuItem("Save Table"))
            {
                std::wstring_view desktopPath = File::GetDesktopPath();
                File::Path out;
                if (File::ShowSaveFileDialog(NULL, L"저장할 경로를 선택하세요.", desktopPath.data(),
                                             L"WeaponTable.WpTable", {{L"무기 테이블 파일\0", L"*.WpTable\0"}}, out))
                {
                    bool isWrite = true;
                    if (std::filesystem::exists(out))
                    {
                        int result =
                            MessageBoxW(UmApplication.GetHwnd(),      // 부모 윈도우 핸들
                                        L"파일을 덮어쓰시겠습니까?",  // 메시지 내용 (설명)
                                        L"이미 존재하는 파일입니다.", // 메시지 박스 제목
                                        MB_OKCANCEL | MB_ICONQUESTION // 버튼 구성 (확인/취소) 및 아이콘(물음표)
                            );
                        switch (result)
                        {
                        case IDOK:
                            isWrite = true;
                            break;
                        case IDCANCEL:
                        default:
                            isWrite = false;
                            break;
                        }
                    }

                    if (isWrite)
                    {
                        std::ofstream ofs(out, std::ios::trunc);
                        if (ofs.is_open())
                        {
                            ofs << SaveWeaponTable();
                            ofs.close();
                        }
                    }
                }
            }
            if (ImGui::MenuItem("Load Table"))
            {
                std::wstring_view desktopPath = File::GetDesktopPath();
                std::vector<File::Path> out;
                if (File::ShowOpenFileDialog(NULL, L"로드할 파일을 선택하세요.", desktopPath.data(),
                                             {{L"무기 테이블 파일\0", L"*.WpTable\0"}}, false, out))
                {
                    if (std::filesystem::exists(out.front()))
                    {
                        std::ifstream ifs(out.front());

                        if (ifs.is_open())
                        {
                            std::string content((std::istreambuf_iterator<char>(ifs)),
                                                std::istreambuf_iterator<char>());
                            LoadWeaponTable(content);
                            ifs.close();
                        }
                    }
                }
                gameObject->GetScene().IsDirty = true;
            }
            ImGui::EndMenuBar();
        }
        ImGuiTableEditor();
        ImGui::End();
    }
}

void WeaponTableComponent::ImGuiTableEditor() 
{
    if (ImGui::TreeNodeEx("Weapon Table", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::BeginTable("Weapon Stats", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Type");
            ImGui::TableSetupColumn("Hit Damage");
            ImGui::TableSetupColumn("Critical Damage");
            ImGui::TableSetupColumn("Speed");
            ImGui::TableSetupColumn("Attack Count");
            ImGui::TableHeadersRow();

            for (auto& [key, weapon] : _weaponTable)
            {
                auto RightClickContext = [&]() {
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

                int itemID = 0;
                ImGui::PushStyleColor(ImGuiCol_Text, GetWeaponTypeColor(weapon.Type));
                ImGui::PushID(itemID++);
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ReflectHelper::ImGuiDraw::Private::InputAuto(weapon.Name, UmCore->ImGuiDrawPropertysSetting);
                    RightClickContext();
                    ImGui::TableSetColumnIndex(1);
                    ReflectHelper::ImGuiDraw::Private::InputAuto(weapon.Type, UmCore->ImGuiDrawPropertysSetting);
                    RightClickContext();
                    ImGui::TableSetColumnIndex(2);
                    ReflectHelper::ImGuiDraw::Private::InputAuto(weapon.HitDamage, UmCore->ImGuiDrawPropertysSetting);      
                    RightClickContext();
                    ImGui::TableSetColumnIndex(3);
                    ReflectHelper::ImGuiDraw::Private::InputAuto(weapon.CriticalDamage, UmCore->ImGuiDrawPropertysSetting);
                    RightClickContext();
                    ImGui::TableSetColumnIndex(4);
                    ReflectHelper::ImGuiDraw::Private::InputAuto(weapon.Speed, UmCore->ImGuiDrawPropertysSetting);
                    RightClickContext();
                    ImGui::TableSetColumnIndex(5);
                    ReflectHelper::ImGuiDraw::Private::InputAuto(weapon.AttackCount, UmCore->ImGuiDrawPropertysSetting);       
                    RightClickContext();
                }
                ImGui::PopID();
                ImGui::PopStyleColor(1);
            }     
            ImGui::EndTable();
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
            EraseWeapon(_weaponTable[_imguiEvent.DeleteTableBuffer]);
            _imguiEvent.DeleteTableBuffer = STR_NULL;
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
    _weaponTable.clear();
    for (auto& weapon : ReflectFields->_tableDatas)
    {
        WeaponStats stats;
        stats.DeserializedReflectFields(weapon);
        InsertWeapon(stats);
    }
}
