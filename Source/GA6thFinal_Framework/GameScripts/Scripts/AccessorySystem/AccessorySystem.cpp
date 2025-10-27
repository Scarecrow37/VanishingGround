#include "pchScripts.h"
#include "AccessorySystem.h"
#include "TurnSystem/TurnAction/TurnActionFactory.h"
#include "ExcelDataSystem/ExcelDataSystem.h"
#include "UI/Views/Accessories/AccessoriesView.h"

UMREAL_COMPONENT(AccessorySystem)

using namespace u8_literals;

AccessorySystem::AccessorySystem()
{

}

AccessorySystem::~AccessorySystem()
{

}

bool AccessorySystem::EquipAccessory(const AccessoryElement& accessory)
{
    auto [iter, result] = _playerAccessoryItemSet.insert(accessory.AccessoryID);
    if (result)
    {
        _playerAccessoryItems.push_back(accessory);
    }
    return result;
}

bool AccessorySystem::UnequipAccessory(const AccessoryElement& accessory)
{
    size_t eraseCount = _playerAccessoryItemSet.erase(accessory.AccessoryID);
    bool   result     = 0 < eraseCount;
    if (result)
    {
        _playerAccessoryItems.erase(accessory);
    }
    return result;
}

void AccessorySystem::ImGuiDrawPropertysEvent() 
{
#ifdef _UMEDITOR
    if (ImGui::Button("Table Editor"))
    {
        _editorOnly.ShowTableEditor = true;
    }

    if (_editorOnly.ShowTableEditor)
    {
        ImGuiViewport* viewPort = ImGui::GetMainViewport();
        ImVec2         center   = viewPort->GetCenter();
        ImVec2         size     = viewPort->Size * 0.75f;
        ImGui::SetNextWindowPos(center, ImGuiCond_Once, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(size, ImGuiCond_FirstUseEver);
        ImGui::Begin("Accessory Table Editor##1191B534-B4B7-425C-8638-EFE3B662DB9C", &_editorOnly.ShowTableEditor,
                     ImGuiWindowFlags_MenuBar);

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::MenuItem("Save Table"))
            {
                std::wstring_view desktopPath = File::GetDesktopPath();
                File::Path        out;
                if (File::ShowSaveFileDialog(NULL, L"저장할 경로를 선택하세요.", desktopPath.data(),
                                             L"AsTable.AsTable", {{L"장신구 테이블 파일\0", L"*.AsTable\0"}}, out))
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
                            ofs << SaveAccessoryTable();
                            ofs.close();
                        }
                    }
                }
            }
            if (ImGui::MenuItem("Load Table"))
            {
                std::wstring_view       desktopPath = File::GetDesktopPath();
                std::vector<File::Path> out;
                if (File::ShowOpenFileDialog(NULL, L"로드할 파일을 선택하세요.", desktopPath.data(),
                                             {{L"장신구 테이블 파일\0", L"*.AsTable\0"}}, false, out))
                {
                    if (std::filesystem::exists(out.front()))
                    {
                        std::ifstream ifs(out.front());

                        if (ifs.is_open())
                        {
                            std::string content((std::istreambuf_iterator<char>(ifs)),
                                                std::istreambuf_iterator<char>());
                            LoadAccessoryTable(content);
                            ifs.close();
                        }
                    }
                }
                gameObject->GetScene().IsDirty = true;
            }
            if (ImGui::MenuItem("Excel Parser"))
            {
                auto ParserFunc = [&](ExcelDataBase& dataBase) 
                {
                    constexpr std::array<std::u8string_view, 3> keyInfos{u8"ID", u8"Name", u8"Rarity"};

                    std::unordered_set<std::string> validTargets;
                    size_t rowCount = dataBase.RowCount();
                    for (size_t row = 0; row < rowCount; row++)
                    {
                        AccessoryElement temp;
                        bool             result = true;

                        for (auto& key : keyInfos)
                        {
                            std::string_view data = dataBase.FindData(row, key);
                            result &= ExcelAccessoryElement(temp, (const char*)key.data(), data.data());
                        }
                        const std::string& name = temp.AccessoryName;
                        if (name != STR_NULL)
                        {
                            auto nameFindIter = _elementTable.find(name);
                            bool findName     = nameFindIter != _elementTable.end();
                            auto idFindIter = std::ranges::find_if(_elementTableOrderID, [&temp](AccessoryElement* accessory) 
                            {
                                int currID = accessory->AccessoryID;
                                int tempID = temp.AccessoryID;
                                return currID == tempID;
                            });
                            bool findID = idFindIter != _elementTableOrderID.end();
                            if (false == findName && false == findID)
                            {
                                // 없으면 새로 생성
                                InsertAccessory(temp);
                            }
                            else if (findName)
                            {
                                // 이미 있으면 액션 제외하고 복사
                                std::vector<std::pair<std::string, std::string>> originActions = nameFindIter->second.ReflectFields->Actions;
                                *nameFindIter->second.ReflectFields = *temp.ReflectFields;
                                nameFindIter->second.ReflectFields->Actions = std::move(originActions);
                            }
                            else if (findID)
                            {
                                AccessoryElement*  accessory = *idFindIter;
                                const std::string& tempName  = temp.AccessoryName;
                                RenameAccessory(*accessory, tempName);
                                if (auto findIter = _elementTable.find(tempName); findIter != _elementTable.end())
                                {
                                    accessory = &findIter->second;

                                    std::vector<std::pair<std::string, std::string>> originActions = accessory->ReflectFields->Actions;
                                    *accessory->ReflectFields         = *temp.ReflectFields;
                                    accessory->ReflectFields->Actions = std::move(originActions);
                                }
                            }
                            if (false == result)
                            {
                                // 잘못된 데이터는 알림 팝업
                                AccessoryElement& element = _elementTable[name];
                                _editorOnly.DirtyAccessoryQueue.push(&element);
                            }                  
                            if (auto [iter, insertResult] = validTargets.insert(name); false == insertResult)
                            {
                                std::string message = "\"";
                                message += name;
                                message += (const char*)u8"\" 는 중복된 장신구 이름입니다.";
                                UmLogger.Log(LogLevel::LEVEL_WARNING, message);
                            }
                        }                      
                    }      
                   
                    std::vector<AccessoryElement> eraseTargets;
                    for (auto& accessory : _elementTableOrderID)
                    {
                        const std::string& name = accessory->AccessoryName;
                        if (validTargets.find(name) == validTargets.end())
                        {
                            eraseTargets.emplace_back(*accessory);
                        }
                    }
                    for (auto& target : eraseTargets)
                    {
                        EraseAccessory(target);
                    }
                };

                if (ExcelDataSystem* dataSystem = SingletonComponent<ExcelDataSystem>::GetInstance())
                {
                    if (std::unique_ptr<ExcelDataBase> dataBase = dataSystem->FindExcelDataBase(u8"장신구"))
                    {
                        ParserFunc(*dataBase);
                    }
                }
            }
            ImGui::EndMenuBar();
        }
        ImGuiTableEditor();
        ImGuiDrawExcelParser();
        ImGui::End();
    }
    ImGuiDrawPlayerAccsessoryItems();
#endif
}

void AccessorySystem::ImGuiTableEditor()
{
#ifdef _UMEDITOR
    if (ImGui::BeginTable("Accessory Stats", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableSetupColumn((const char*)u8"ID");     // ID,
        ImGui::TableSetupColumn((const char*)u8"이름");   // Name,
        ImGui::TableSetupColumn((const char*)u8"등급");   // Grade,
        ImGui::TableSetupColumn((const char*)u8"Action"); // Action
        ImGui::TableHeadersRow();

        int itemID = 0;
        for (auto& element : _elementTableOrderID)
        {
            const std::string& key       = element->AccessoryName;
            AccessoryElement&  accessory = *element;

            auto RightClickContext = [&]() 
            {
                if (ImGui::BeginPopupContextItem())
                {
                    if (ImGui::MenuItem("Delete"))
                    {
                        _editorOnly.DeleteTableBuffer = key;
                        _editorOnly.OpenDeletePopup   = true;
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
                    setting.ShowName      = false;
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
                    if (ReflectHelper::ImGuiDraw::Private::InputAuto(accessory.AccessoryID, setting))
                    {
                        SortTableIDOrder();
                    }
                }
                ImGui::TableSetColumnIndex(1);
                {
                    static std::string renameBuffer;
                    const std::string  originName = accessory.AccessoryName;
                    renameBuffer                  = originName;
                    bool input                    = ImGui::InputText("##name", &renameBuffer);
                    if (input)
                    {
                        if (ImGui::IsItemDeactivatedAfterEdit())
                        {
                            if (renameBuffer != originName)
                            {
                                _editorOnly.SelectAccessory = &accessory;
                                _editorOnly.RenameFunc      = [this, renameBuffer = renameBuffer, selectAccessory = _editorOnly.SelectAccessory]() 
                                {
                                    RenameAccessory(*selectAccessory, renameBuffer);
                                };
                                _editorOnly.SelectAccessory = nullptr;
                            };
                        }
                    }
                    RightClickContext();
                };

                ImGui::PushStyleColor(ImGuiCol_Text, accessory.GetGradeColor());
                DrawColumnProperty(accessory.Grade, 2);
                ImGui::PopStyleColor();
                ImGui::TableSetColumnIndex(3);
                {
                    if (false == accessory._actions.empty())
                    {
                        for (size_t i = 0; i < accessory._actions.size(); ++i)
                        {
                            auto& action = accessory._actions[i];
                            if (_editorOnly.ShowActionEditor.size() <= i)
                            {
                                _editorOnly.ShowActionEditor.resize(i + 1);
                            }
                            bool showEditor = _editorOnly.ShowActionEditor[i];
                            TurnAction::ImGuiDrawActionMaker(key + std::to_string(i), action, showEditor);
                            _editorOnly.ShowActionEditor[i] = showEditor;
                        }
                    }
                    if (ImGui::Button("Push"))
                    {
                        accessory._actions.emplace_back();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Pop"))
                    {
                        if (false == accessory._actions.empty())
                        {
                            accessory._actions.pop_back();
                        }
                    }
                }
            }
            ImGui::PopID();
        }
        ImGui::EndTable();

        if (_editorOnly.RenameFunc)
        {
            _editorOnly.RenameFunc();
            _editorOnly.RenameFunc = nullptr;
        }
    }

    if (_editorOnly.OpenDeletePopup)
    {
        _editorOnly.OpenDeletePopup = false;
        ImGui::OpenPopup("Accessory Table Delete Modal Popup");
    }

    if (ImGui::BeginPopupModal("Accessory Table Delete Modal Popup"))
    {
        ImGui::Text((const char*)u8"이 작업은 되돌릴 수 없습니다.");
        ImGui::Text("%s", _editorOnly.DeleteTableBuffer.c_str());
        ImGui::SameLine();
        ImGui::Text((const char*)u8"정말 삭제하시겠습니까?");
        ImGui::Separator();
        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            EraseAccessory(_elementTable[_editorOnly.DeleteTableBuffer]);
            _editorOnly.DeleteTableBuffer = STR_NULL;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)) || ImGui::IsKeyReleased(ImGuiKey_Escape))
        {
            _editorOnly.DeleteTableBuffer = STR_NULL;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    static std::string newAccessoryName;
    ImGui::InputText("##New Accessory", &newAccessoryName);
    ImGui::SameLine();
    if (ImGui::Button("New Accessory"))
    {
        if (false == newAccessoryName.empty())
        {
            AccessoryElement element;
            element.SetName(newAccessoryName);
            bool result = InsertAccessory(element);
            if (true == result)
            {
                newAccessoryName.clear();
            }
        }
    }
#endif
}

void AccessorySystem::ImGuiDrawExcelParser()
{
#ifdef _UMEDITOR
    if (ImGui::BeginPopupModal(u8"알림##Dirty Accessory Popup"_c_str))
    {
        auto PopDirtyAccessoryElement = [this]() {
            _editorOnly.ShowDirtyAccessoryPopup = false;
            _editorOnly.DirtyAccessoryQueue.pop();
        };

        ImGui::Text(u8"올바르지 않은 형식입니다. 직접 입력해주세요."_c_str);
        AccessoryElement& element = *_editorOnly.DirtyAccessoryQueue.front();
        element.ImGuiDrawPropertys();
        ImGui::Separator();
        if (ImGui::Button("OK"))
        {
            PopDirtyAccessoryElement();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            PopDirtyAccessoryElement();
            EraseAccessory(element);
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (false == _editorOnly.DirtyAccessoryQueue.empty() && false == _editorOnly.ShowDirtyAccessoryPopup)
    {
        ImGui::OpenPopup(u8"알림##Dirty Accessory Popup"_c_str);
        _editorOnly.ShowDirtyAccessoryPopup = true;
    }
#endif
}

void AccessorySystem::ImGuiDrawPlayerAccsessoryItems() 
{
#ifdef _UMEDITOR
    if (ImGui::TreeNode("Player Accsessories"))
    {
        auto AccessorySelectCombo = [this](const char* prevValue) 
        {
            AccessoryElement* select = nullptr;
            if (ImGui::BeginCombo("##Accessory select", prevValue))
            {
                for (auto& tableAccessory : GetAccessoryTableElements())
                {
                    if (HasPlayerAccessory(tableAccessory->AccessoryID))
                    {
                        continue;
                    }
                    ImGui::PushStyleColor(ImGuiCol_Text, tableAccessory->GetGradeColor());
                    const std::string& name = tableAccessory->AccessoryName;
                    if (ImGui::Selectable(name.c_str(), name == prevValue))
                    {
                        select = tableAccessory;
                    }
                    ImGui::PopStyleColor();
                }
                ImGui::EndCombo();
            } 
            return select;
        };

        AccessoryElement* unequipTarget = nullptr;
        const auto& items =_playerAccessoryItems;
        for (size_t i = 0; i < items.size(); ++i)
        {
            const auto& accessory = _playerAccessoryItems.at(i);
            ImGui::PushID(&accessory);
            ImGui::PushStyleColor(ImGuiCol_Text, accessory.GetGradeColor());
            {
                const std::string& name = accessory.AccessoryName;
                AccessoryElement*  change = AccessorySelectCombo(name.c_str());
                if (change)
                {
                    _playerAccessoryItems.at(i, [&](AccessoryElement& element) 
                    { 
                        element = *change;
                    });
                }

                ImGui::SameLine();
                if (ImGui::Button("Unequip"))
                {
                    _playerAccessoryItems.at(i, [&](AccessoryElement& element) 
                    {
                        unequipTarget = &element;
                    });
                }
            }
            ImGui::PopStyleColor();
            ImGui::PopID();
        }
        if (nullptr != unequipTarget)
        {
            UnequipAccessory(*unequipTarget);
        }

        static thread_local std::string equipAccessoryName = STR_NULL;
        if (ImGui::Button("Equip"))
        {
            std::unique_ptr<AccessoryElement> newAccessory = TryMakeAccessoryToName(equipAccessoryName);
            if (newAccessory)
            {
                if (EquipAccessory(*newAccessory))
                {
                    equipAccessoryName = STR_NULL;
                }
            }
        }
        ImGui::SameLine();
        AccessoryElement* equipTarget = AccessorySelectCombo(equipAccessoryName.c_str());
        if (equipTarget)
        {
            equipAccessoryName = equipTarget->AccessoryName;
        }
        ImGui::TreePop();
    }
#endif
}

bool AccessorySystem::ExcelAccessoryElement(AccessoryElement& element, const std::string& key, const std::string& data)
{
    if (false == key.empty())
    {
        try
        {
            std::wstring wcharKey = U8ToWString(key);
            if (L"ID" == wcharKey)
            {
                element.AccessoryID = std::stoi(data);
            }
            else if (L"Name" == wcharKey)
            {
                if (false == data.empty())
                {
                    element.SetName(data);
                }
            }
            else if (L"Rarity" == wcharKey)
            {
                int rarity = std::stoi(data);

                if (AccessorySystem::GetGradeID(AccessoryGrade::COMMON) == rarity)
                {
                    element.Grade = AccessoryGrade::COMMON;
                }
                else if (AccessorySystem::GetGradeID(AccessoryGrade::RARE) == rarity)
                {
                    element.Grade = AccessoryGrade::RARE;
                }
                else if (AccessorySystem::GetGradeID(AccessoryGrade::BIZARRE) == rarity)
                {
                    element.Grade = AccessoryGrade::BIZARRE;
                }
                else if (AccessorySystem::GetGradeID(AccessoryGrade::LEGENDARY) == rarity)
                {
                    element.Grade = AccessoryGrade::LEGENDARY;
                }
                else
                {
                    return false;
                }
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

void AccessorySystem::SerializedReflectEvent() 
{
    ElementTableSerialized();
    PlayerAccessoriesSerialized();
}

void AccessorySystem::DeserializedReflectEvent() 
{
    ElementTableDeserialized();
    PlayerAccessoriesDeserialized();
}

void AccessorySystem::Reset()
{
    _singletonComponent.SetSingleTon();
}

void AccessorySystem::Awake() 
{
    if (_singletonComponent.TrySingleTon())
    {
        UmWatcher.Register<AccessoriesViewModel>(AccessoriesView::VIEW_KEY, _playerAccessoryItems);
    }
}

void AccessorySystem::OnDestroy() 
{
    if (_singletonComponent.IsSingleTon())
    {
        UmWatcher.Unregister<AccessoriesViewModel>(AccessoriesView::VIEW_KEY);
    }
}

void AccessorySystem::ElementTableSerialized() 
{
    ReflectFields->ElementTableData.clear();
    for (auto& [key, element] : _elementTable)
    {
        ReflectFields->ElementTableData.emplace_back(key, element.SerializedReflectFields());
    }
}

void AccessorySystem::ElementTableDeserialized() 
{
    _elementTable.clear();
    _elementTableOrderID.clear();
    for (auto& [key, data] : ReflectFields->ElementTableData)
    {
        AccessoryElement element;
        element.DeserializedReflectFields(data);
        InsertAccessory(element);
    }
    SortTableIDOrder();
}

void AccessorySystem::PlayerAccessoriesSerialized() 
{
    ReflectFields->PlayerAccessoriesNames.clear();
    const auto& elements = _playerAccessoryItems;
    for (auto& element : elements)
    {
        const std::string& name = element.AccessoryName;
        ReflectFields->PlayerAccessoriesNames.push_back(name);
    }
}

void AccessorySystem::PlayerAccessoriesDeserialized() 
{
    _playerAccessoryItems.clear();
    _playerAccessoryItemSet.clear();
    for (auto& name : ReflectFields->PlayerAccessoriesNames)
    {
        std::unique_ptr<AccessoryElement> element = TryMakeAccessoryToName(name);
        if (element)
        {
            EquipAccessory(*element);
        }
    }
}

bool AccessorySystem::RenameAccessory(AccessoryElement& accessory, const std::string& newName)
{
    if (_elementTable.find(newName) == _elementTable.end())
    {
        AccessoryElement newElement = accessory;
        if (EraseAccessory(accessory))
        {
            newElement.SetName(newName);
            if (InsertAccessory(newElement))
            {
                return true;
            }
        }
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"존재하지 않는 장신구 입니다.");
        return false;
    }
    UmLogger.Log(LogLevel::LEVEL_WARNING, u8"이미 존재하는 이름입니다.");
    return false;
}

bool AccessorySystem::InsertAccessory(AccessoryElement& accessory)
{
    const std::string& name = accessory.AccessoryName;
    auto [iter, result] = _elementTable.try_emplace(name, accessory);
    if (result)
    {
        _elementTableOrderID.push_back(&iter->second);
        SortTableIDOrder();
        return true;
    }
    else
    {
        return false;
    }
}

bool AccessorySystem::EraseAccessory(AccessoryElement& accessory)
{
    const std::string& name = accessory.AccessoryName;
    auto findIter = _elementTable.find(name);
    if (findIter != _elementTable.end())
    {
        std::erase_if(_elementTableOrderID, [&findIter](AccessoryElement* element) 
        { 
            return element == &findIter->second;
        });
        _elementTable.erase(findIter);
        SortTableIDOrder();
        return true;
    }
    else
    {
        return false;
    }
}

void AccessorySystem::SortTableIDOrder()
{
    std::ranges::sort(_elementTableOrderID, [](AccessoryElement* elementA, AccessoryElement* elementB) 
    {
        int idA = elementA->AccessoryID; 
        int idB = elementB->AccessoryID; 
        return idA < idB;
    });
}

std::string AccessorySystem::SaveAccessoryTable()
{
    ElementTableSerialized();
    return rfl::json::write(ReflectFields->ElementTableData);
}

bool AccessorySystem::LoadAccessoryTable(const std::string& data)
{
    auto result = rfl::json::read<decltype(ReflectFields->ElementTableData)>(data.data());
    if (result)
    {
        ReflectFields->ElementTableData = result.value();
        ElementTableDeserialized();
        return true;
    }
    return false;
}
