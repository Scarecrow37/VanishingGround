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

const WeaponElement* WeaponTableComponent::GetWeaponToName(std::string_view name)
{
    WeaponElement* result   = nullptr;
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
        WeaponElement& newWeapon = _weaponTable[newName];
        newWeapon = weapon; 
        newWeapon.Stats.SetName(newName); // 이름 변경

        const std::string& prevName = weapon.Stats.WeaponName;
        _weaponTable.erase(prevName);    //기존 삭제
        result = true;
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
        _weaponTable.erase(name);
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
            ImGui::MenuItem("Excel Parser", "", &_imguiEvent.ShowExcelParser);
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
        for (auto& [key, weapon] : _weaponTable)
        {
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

                DrawColumnProperty(weapon.Stats.WeaponID, 0);
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
    auto DirtyWeaponModalPopup = [this]() 
    {
        if (ImGui::BeginPopupModal(u8"알림##Dirty Weapon Popup"_c_str))
        {
            ImGui::Text(u8"올바르지 않은 형식입니다. 직접 입력해주세요."_c_str);
            WeaponElement& element = *_imguiEvent.DirtyWeaponElementQueue.front();
            element.ImGuiDrawPropertys();
            ImGui::Separator();
            if (ImGui::Button("OK"))
            {
                _imguiEvent.ShowDirtyWeaponPopup = false;
                _imguiEvent.DirtyWeaponElementQueue.pop();
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                _imguiEvent.ShowDirtyWeaponPopup = false;
                _imguiEvent.DirtyWeaponElementQueue.pop();
                EraseWeapon(element);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    };

    if (_imguiEvent.ShowExcelParser)
    {
        ImGui::Begin("Excel Parser##12487AA8-BA7A-43E8-90A6-EBC10DAE14FC", &_imguiEvent.ShowExcelParser,
                     ImGuiWindowFlags_MenuBar);
        {
            ImGui::PushID(this);
            ImGuiDrawExcelParserMenuBar();
            DirtyWeaponModalPopup();
            if (false == _imguiEvent.SheetDatas.empty())
            {
                if (true == _imguiEvent.DirtyWeaponElementQueue.empty())
                {
                    _imguiEvent.ShowExcelParser = false;
                }
                else if (false == _imguiEvent.ShowDirtyWeaponPopup)
                {
                    ImGui::OpenPopup(u8"알림##Dirty Weapon Popup"_c_str);
                    _imguiEvent.ShowDirtyWeaponPopup = true;
                }
            }
            else if (true == _imguiEvent.SheetNames.empty())
            {
                ImGui::Text((const char*)u8"엑셀 파일을 로드해주세요.");
            }
            else
            {
                ImGui::Text(u8"파싱할 시트를 선택하세요."_c_str);
                if (ImGui::BeginCombo("##{A4CAA356-B858-4BFF-85E8-52E3B270A7D2}", _imguiEvent.SelectSheetName.c_str()))
                {
                    for (auto& name : _imguiEvent.SheetNames)
                    {
                        if (ImGui::Selectable(name.c_str(), _imguiEvent.SelectSheetName == name))
                        {
                            _imguiEvent.SelectSheetName = name;
                        }
                    }
                    ImGui::EndCombo();
                }
                if (ImGui::Button("Ok") && false == _imguiEvent.SelectSheetName.empty())
                {
                    auto& doc = *_imguiEvent.ExcelDoc;
                    auto  workBook = doc.workbook();
                    auto  workSheet = workBook.worksheet(_imguiEvent.SelectSheetName.c_str());

                    auto [keyRow, keyColum] = OpenXLSXHelper::FindRowColumnToData(workSheet, u8"이름"_c_str);
                    if (OpenXLSXHelper::IsFindSuccess(keyRow, keyColum))
                    {        
                        //파싱
                        _imguiEvent.SheetDatas.clear();
                        _imguiEvent.SheetDatas = OpenXLSXHelper::ParseSheetWithColumnKeys(workSheet, keyRow);

                        //생성
                        if (false == _imguiEvent.SheetDatas.empty())
                        {                                                
                            for (size_t row = 0; row < _imguiEvent.SheetDatas.front().second.size(); ++row)
                            {
                                bool result = true;
                                WeaponElement temp;
                                for (auto& [key, datas] : _imguiEvent.SheetDatas)
                                {
                                    result &= ExcelToWeaponElement(temp, key, datas[row]);
                                }
                                const std::string& name = temp.Stats.WeaponName;
                                if (name != WeaponStats::DEFAULT_NAME)
                                {
                                    auto findWeaponIter = _weaponTable.find(name);
                                    if (findWeaponIter == _weaponTable.end())
                                    {
                                        //없으면 새로 생성
                                        InsertWeapon(temp);
                                    }
                                    else
                                    {
                                        //이미 있으면 스텟만 복사
                                        findWeaponIter->second.Stats = temp.Stats;
                                    }
                                    if (false == result)
                                    {
                                        //잘못된 데이터는 알림 팝업
                                        WeaponElement& element = _weaponTable[name];
                                        _imguiEvent.DirtyWeaponElementQueue.push(&element);
                                    }
                                }
                            }
                        }
                    }                  
                }
            }
            ImGui::PopID();
        }
        ImGui::End();
    }

    if (_imguiEvent.ExcelDoc && _imguiEvent.ShowExcelParser == false)
    {
        if (_imguiEvent.ExcelDoc->isOpen())
        {
            _imguiEvent.ExcelDoc->close();
        }
        _imguiEvent.ExcelDoc.reset();
        _imguiEvent.SheetNames.clear();
        _imguiEvent.SelectSheetName.clear();
        _imguiEvent.SheetDatas.clear();
    }
#endif
}

void WeaponTableComponent::ImGuiDrawExcelParserMenuBar() 
{
#ifdef _UMEDITOR
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::MenuItem("Load Excel Table"))
        {
            std::wstring_view       desktopPath = File::GetDesktopPath();
            std::vector<File::Path> out;
            if (File::ShowOpenFileDialog(NULL, L"로드할 파일을 선택하세요.", desktopPath.data(),
                                         {{L"무기 테이블 파일\0", L"*.xlsm\0"}}, false, out))
            {
                if (false == out.empty())
                {
                    if (nullptr == _imguiEvent.ExcelDoc)
                    {
                        _imguiEvent.ExcelDoc.reset(new OpenXLSX::XLDocument);
                    }
                    _imguiEvent.SheetNames.clear();
                    _imguiEvent.SelectSheetName.clear();
                    _imguiEvent.SheetDatas.clear();
                    _imguiEvent.ExcelDoc->open(out.front().generic_string());
                    auto& doc = *_imguiEvent.ExcelDoc;
                    if (doc.isOpen())
                    {
                        auto workBook          = doc.workbook();
                        _imguiEvent.SheetNames = workBook.sheetNames();
                    }
                    gameObject->GetScene().IsDirty = true;
                }
            }
        }
        ImGui::EndMenuBar();
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
            if (wcharKey.find(L"ID") != std::wstring::npos)
            {
                stats.WeaponID = std::stoi(data);
            }
            else if (wcharKey.find(L"이름") != std::wstring::npos)
            {
                if (false == data.empty())
                {
                    stats.SetName(data);
                }             
            }
            else if (wcharKey.find(L"타입") != std::wstring::npos)
            {
                if (u8"검"_c_str == data)
                {
                    stats.Type = WeaponType::SWORD;
                }
                else if (u8"단검"_c_str == data)
                {
                    stats.Type = WeaponType::DAGGER;
                }
                else if (u8"대형망치"_c_str == data)
                {
                    stats.Type = WeaponType::WARHAMMER;
                }
                else
                {
                    return false;
                }
            }
            else if (wcharKey.find(L"등급") != std::wstring::npos)
            {
                if (u8"일반"_c_str == data)
                {
                    stats.Grade = WeaponGrade::COMMON;
                }
                else if (u8"희귀"_c_str == data)
                {
                    stats.Grade = WeaponGrade::RARE;
                }
                else if (u8"신비"_c_str == data)
                {
                    stats.Grade = WeaponGrade::BIZARRE;
                }
                else if (u8"전설"_c_str == data)
                {
                    stats.Grade = WeaponGrade::LEGENDARY;
                }
                else
                {
                    return false;
                }       
            }
            else if (wcharKey.find(L"일격 공격력") != std::wstring::npos)
            {
                stats.HitDamage = std::stoi(data);
            }
            else if (wcharKey.find(L"치명타 공격력") != std::wstring::npos)
            {
                stats.CriticalDamage = std::stoi(data);
            }
            else if (wcharKey.find(L"공격 횟수") != std::wstring::npos)
            {
                stats.AttackCount = std::stoi(data);
            }
            else if (wcharKey.find(L"속도") != std::wstring::npos)
            {
                stats.Speed = std::stoi(data);
            }
            else if (wcharKey.find(L"1 공격 당 연격 부여량") != std::wstring::npos)
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
    for (auto& weapon : ReflectFields->_tableDatas)
    {
        WeaponElement element;
        element.DeserializedReflectFields(weapon);
        InsertWeapon(element);
    }
}
