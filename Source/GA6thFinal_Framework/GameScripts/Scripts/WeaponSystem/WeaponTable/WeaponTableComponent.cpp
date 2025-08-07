#include "pchScripts.h"
#include "WeaponTableComponent.h"

using namespace u8_literals;

WeaponTableComponent::WeaponTableComponent() = default;
WeaponTableComponent::~WeaponTableComponent()
{
    if (static_instance == this)
    {
        static_instance = nullptr;
    }
}

const WeaponElement* WeaponTableComponent::GetWeaponToName(const std::string& name)
{
    WeaponElement* result = nullptr;
    auto findIter = _weaponTable.find(name);
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
        _weaponTableIdOrder.clear();
        for (auto& weapon : ReflectFields->_tableDatas)
        {
            WeaponElement element;
            element.DeserializedReflectFields(weapon);
            InsertWeapon(element);
        }
    } 
    return result;
}

bool WeaponTableComponent::RenameWeapon(WeaponElement& weapon, const std::string& newName)
{
    bool result = false;
    auto findIter = _weaponTable.find(newName);
    if (findIter == _weaponTable.end())
    {
        WeaponElement newWeapon = weapon;
        newWeapon.Stats.SetName(newName); 
        if (InsertWeapon(newWeapon))
        {
            if (EraseWeapon(weapon))
            {
                result = true;
            }
            else
            {
                EraseWeapon(newWeapon);
            }
        }
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, (const char*)u8"이미 존재하는 Weapon 이름 입니다.");
    }
    return result;
}

bool WeaponTableComponent::InsertWeapon(WeaponElement& weapon)
{
    bool result = false;
    const std::string& name     = weapon.Stats.WeaponName;
    auto             findIter = _weaponTable.find(name);
    if (findIter == _weaponTable.end())
    {
        WeaponElement& newWeapon = _weaponTable[name];
        newWeapon = weapon;
        ClampMultiplierStats(newWeapon);
        _weaponTableIdOrder.push_back(&newWeapon);

        //ID 기준 정렬
        SortTableIDOrder();
        result = true;
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, (const char*)u8"이미 존재하는 Weapon 이름 입니다.");
    }
    return result;
}

bool WeaponTableComponent::EraseWeapon(WeaponElement& weapon)
{
    bool               result   = false;
    const std::string& name     = weapon.Stats.WeaponName;
    auto               findIter = _weaponTable.find(name);
    if (findIter != _weaponTable.end())
    {
        //ID 기준에서 제거
        std::erase(_weaponTableIdOrder, &findIter->second);

        _weaponTable.erase(name);
        result = true;
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, (const char*)u8"존재 하지 않는 Weapon 이름 입니다.");
    }
    return result;
}

void WeaponTableComponent::SortTableIDOrder()
{
    std::ranges::sort(_weaponTableIdOrder,
    [](const WeaponElement* a, const WeaponElement* b) 
    { 
        return a->Stats.WeaponID < b->Stats.WeaponID; 
    });
}

void WeaponTableComponent::ClampMultiplierStats(WeaponElement& weapon) 
{
    float hitDamageMultiplier        = weapon.Stats.HitDamageMultiplier;
    weapon.Stats.HitDamageMultiplier = std::max(1.f, hitDamageMultiplier);

    float criticalDamageMultiplier        = weapon.Stats.CriticalDamageMultiplier;
    weapon.Stats.CriticalDamageMultiplier = std::max(1.f, criticalDamageMultiplier);

    float attackPerChainMultiplier        = weapon.Stats.AttackPerChainMultiplier;
    weapon.Stats.AttackPerChainMultiplier = std::max(1.f, attackPerChainMultiplier);
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
#ifdef _UMEDITOR
    if (ImGui::Button("Table Editor"))
    {
        _imguiEvent.ShowTableEditor = true;
    }
   
    if (_imguiEvent.ShowTableEditor)
    {
        ImGuiViewport* viewPort = ImGui::GetMainViewport();
        ImVec2         center   = viewPort->GetCenter();
        ImVec2         size     = viewPort->Size * 0.75f;
        ImGui::SetNextWindowPos(center, ImGuiCond_Once, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(size, ImGuiCond_FirstUseEver);
        ImGui::Begin("Weapon Table Editor##6A2AE1D2-0061-488B-9629-FBB94B054B0C",
                     &_imguiEvent.ShowTableEditor, ImGuiWindowFlags_MenuBar);

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
            ImGui::MenuItem("Excel Parser", "", &_imguiEvent.ColumnParser.ShowParser);
            ImGui::EndMenuBar();
        }
        ImGuiTableEditor();
        ImGuiDrawExcelParser();
        ImGui::End();
    }
#endif
}

void WeaponTableComponent::ImGuiTableEditor() 
{
#ifdef _UMEDITOR
    if (ImGui::BeginTable("Weapon Stats", 9, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableSetupColumn((const char*)u8"ID");                   // ID,
        ImGui::TableSetupColumn((const char*)u8"이름");                   // Name,
        ImGui::TableSetupColumn((const char*)u8"종류");                   // Type,
        ImGui::TableSetupColumn((const char*)u8"등급");                   // Grade,
        ImGui::TableSetupColumn((const char*)u8"데미지");                 // HitDamage,
        //ImGui::TableSetupColumn((const char*)u8"데미지 배율");            // HitDamageMultiplier,
        ImGui::TableSetupColumn((const char*)u8"치명타 데미지");          // CriticalDamage,
        //ImGui::TableSetupColumn((const char*)u8"치명타 데미지 배율");     // CriticalDamageMultiplier,
        ImGui::TableSetupColumn((const char*)u8"공격 수");                // AttackCount,
        ImGui::TableSetupColumn((const char*)u8"속도");                   // Speed,
        //ImGui::TableSetupColumn((const char*)u8"공격 당 연격 스택");      // AttackPerChain,
        //ImGui::TableSetupColumn((const char*)u8"공격 당 연격 스택 배율"); // AttackPerChainMultiplier,
        ImGui::TableSetupColumn((const char*)u8"Action");                 // Action   

        ImGui::TableHeadersRow();   
        int itemID = 0;
        for (auto& element : _weaponTableIdOrder)
        {
            const std::string& key = element->Stats.WeaponName;
            WeaponElement&     weapon = *element;

            auto RightClickContext = [&]() {
                if (ImGui::BeginPopupContextItem())
                {
                    if (ImGui::MenuItem("Delete"))
                    {
                        _imguiEvent.DeleteTableBuffer = key;
                        _imguiEvent.OpenDeletePopup   = true;
                    }
                    ImGui::EndPopup();
                }
            };
            ImGui::PushID(itemID++);
            {
                static ReflectHelper::ImGuiDraw::InputAutoSetting setting = []() 
                {
                    ReflectHelper::ImGuiDraw::InputAutoSetting setting;
                    setting._float.format = "%.1f";
                    setting.ShowName = false;
                    return setting;
                }();
                ImGui::TableNextRow();
                auto DrawColumnProperty = [&RightClickContext](auto& property, int index, std::string_view toolTip = "") 
                {
                    ImGui::TableSetColumnIndex(index);
                    ReflectHelper::ImGuiDraw::Private::InputAuto(property, setting);
                    if (false == toolTip.empty())
                    {
                        ImGuiHelper::HoveredToolTip(toolTip.data());
                    }
                    RightClickContext();
                };

                ImGui::TableSetColumnIndex(0);
                {
                    if (ReflectHelper::ImGuiDraw::Private::InputAuto(weapon.Stats.WeaponID, setting))
                    {
                        SortTableIDOrder();
                    }
                }
                ImGui::TableSetColumnIndex(1);
                {
                    static std::string renameBuffer;
                    const std::string  originName = weapon.Stats.WeaponName;
                    renameBuffer                  = originName;
                    bool input                    = ImGui::InputText("##name", &renameBuffer);
                    if (input)
                    {
                        if (ImGui::IsItemDeactivatedAfterEdit())
                        {
                            if (renameBuffer != originName)
                            {
                                _imguiEvent.SelectWeapon = &weapon;
                                _imguiEvent.RenameFunc   = [this, renameBuffer = renameBuffer,
                                                          slectWeapon = _imguiEvent.SelectWeapon]() 
                                {
                                    RenameWeapon(*slectWeapon, renameBuffer);
                                };
                                _imguiEvent.SelectWeapon = nullptr;
                            };
                        }
                    }
                    RightClickContext();
                };

                ImGui::PushStyleColor(ImGuiCol_Text, WeaponStats::GetTypeToColor(weapon.Stats.Type));
                DrawColumnProperty(weapon.Stats.Type, 2);
                ImGui::PopStyleColor();

                ImGui::PushStyleColor(ImGuiCol_Text, WeaponStats::GetGradeToColor(weapon.Stats.Grade));
                DrawColumnProperty(weapon.Stats.Grade, 3);
                ImGui::PopStyleColor();

                DrawColumnProperty(weapon.Stats.HitDamage, 4);
                // DrawColumnProperty(weapon.Stats.HitDamageMultiplier,      3);
                DrawColumnProperty(weapon.Stats.CriticalDamage, 5);
                // DrawColumnProperty(weapon.Stats.CriticalDamageMultiplier, 5);
                DrawColumnProperty(weapon.Stats.AttackCount, 6);
                DrawColumnProperty(weapon.Stats.Speed, 7);
                // DrawColumnProperty(weapon.Stats.AttackPerChain,           8);
                // DrawColumnProperty(weapon.Stats.AttackPerChainMultiplier, 9);    
                ImGui::TableSetColumnIndex(8);
                {
                    TurnAction::ImGuiDrawActionMaker(key, weapon._action, weapon._showActionEditor);
                }              
            }
            ImGui::PopID();
        }
        ImGui::EndTable();

        if (_imguiEvent.RenameFunc)
        {
            _imguiEvent.RenameFunc();
            _imguiEvent.RenameFunc = nullptr;
        }
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
        if (ImGui::Button("Cancel", ImVec2(120, 0)) || ImGui::IsKeyReleased(ImGuiKey_Escape))
        {
            _imguiEvent.DeleteTableBuffer = STR_NULL;
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
            WeaponElement element;
            element.Stats.SetName(newWeaponName);
            bool result = InsertWeapon(element);
            if (true == result)
            {
                newWeaponName.clear();
            }
        }
    }
#endif
}

void WeaponTableComponent::ImGuiDrawExcelParser() 
{
#ifdef _UMEDITOR
    if (ImGui::BeginPopupModal(u8"알림##Dirty Weapon Popup"_c_str))
    {
        auto PopDirtyWeaponElement = [this]() 
        {
            _imguiEvent.ShowDirtyWeaponPopup = false;
            _imguiEvent.DirtyWeaponElementQueue.pop();
            if (true == _imguiEvent.DirtyWeaponElementQueue.empty())
            {
                _imguiEvent.ColumnParser.ShowParser = false;
            }
        };

        ImGui::Text(u8"올바르지 않은 형식입니다. 직접 입력해주세요."_c_str);
        WeaponElement& element = *_imguiEvent.DirtyWeaponElementQueue.front();
        element.ImGuiDrawPropertys();
        ImGui::Separator();
        if (ImGui::Button("OK"))
        {
            PopDirtyWeaponElement();    
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            PopDirtyWeaponElement();
            EraseWeapon(element);
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (false == _imguiEvent.DirtyWeaponElementQueue.empty() && false == _imguiEvent.ShowDirtyWeaponPopup)
    {
        ImGui::OpenPopup(u8"알림##Dirty Weapon Popup"_c_str);
        _imguiEvent.ShowDirtyWeaponPopup = true;
    }

    auto ParserFunc = [&](ImGuiColumnSheetParser::ColumnDatas datas) 
    {
        WeaponElement temp;
        bool          result = true;
        for (auto& [key, data] : datas)
        {
            result &= ExcelToWeaponElement(temp, key, data);           
        }
        const std::string& name = temp.Stats.WeaponName;
        if (name != WeaponStats::DEFAULT_NAME)
        {
            auto findWeaponIter = _weaponTable.find(name);
            if (findWeaponIter == _weaponTable.end())
            {
                // 없으면 새로 생성
                InsertWeapon(temp);
            }
            else
            {
                // 이미 있으면 스텟만 복사
                findWeaponIter->second.Stats = temp.Stats;
            }
            if (false == result)
            {
                // 잘못된 데이터는 알림 팝업
                WeaponElement& element = _weaponTable[name];
                _imguiEvent.DirtyWeaponElementQueue.push(&element);
            }
        }
    };
    if (_imguiEvent.ColumnParser.Draw(ParserFunc))
    {
        if (true == _imguiEvent.DirtyWeaponElementQueue.empty())
        {
            _imguiEvent.ColumnParser.ShowParser = false;
        }
    }

#endif
}


bool WeaponTableComponent::ExcelToWeaponElement(WeaponElement& element, const std::string& key, const std::string& data)
{
    if (false == key.empty())
    {
        try
        {
            std::wstring wcharKey = U8ToWString(key);
            WeaponStats& stats = element.Stats;
            if (L"ID" == wcharKey)
            {
                stats.WeaponID = std::stoi(data);
            }
            else if (L"Name" == wcharKey)
            {
                if (false == data.empty())
                {
                    stats.SetName(data);
                }             
            }
            else if (L"Type" == wcharKey)
            {
                if ("Sword" == data)
                {
                    stats.Type = WeaponType::SWORD;
                }
                else if ("Dagger" == data)
                {
                    stats.Type = WeaponType::DAGGER;
                }
                else if ("GiantHammer" == data)
                {
                    stats.Type = WeaponType::WARHAMMER;
                }
                else
                {
                    return false;
                }
            }
            else if (L"Rarity" == wcharKey)
            {
                int rarity = std::stoi(data);
                if (WeaponStats::GetGradeID(WeaponGrade::COMMON) == rarity)
                {
                    stats.Grade = WeaponGrade::COMMON;
                }
                else if (WeaponStats::GetGradeID(WeaponGrade::RARE) == rarity)
                {
                    stats.Grade = WeaponGrade::RARE;
                }
                else if (WeaponStats::GetGradeID(WeaponGrade::BIZARRE) == rarity)
                {
                    stats.Grade = WeaponGrade::BIZARRE;
                }
                else if (WeaponStats::GetGradeID(WeaponGrade::LEGENDARY) == rarity)
                {
                    stats.Grade = WeaponGrade::LEGENDARY;
                }
                else
                {
                    return false;
                }       
            }
            else if (L"HitDamage" == wcharKey)
            {
                stats.HitDamage = std::stoi(data);
            }
            else if (L"CritDamage" == wcharKey)
            {
                stats.CriticalDamage = std::stoi(data);
            }
            else if (L"AttackCount" == wcharKey)
            {
                stats.AttackCount = std::stoi(data);
            }
            else if (L"Speed" == wcharKey)
            {
                stats.Speed = std::stoi(data);
            }
            else if (L"giveChain" == wcharKey)
            {
                stats.AttackPerChain = std::stoi(data);
            }
        }
        catch (const std::invalid_argument&)
        {
            return false;
        }
        catch (const std::out_of_range&)
        {
            return false;
        }
    }
    return true;
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
    _weaponTableIdOrder.clear();
    for (auto& weapon : ReflectFields->_tableDatas)
    {
        WeaponElement element;
        element.DeserializedReflectFields(weapon);
        InsertWeapon(element);
    }
}
