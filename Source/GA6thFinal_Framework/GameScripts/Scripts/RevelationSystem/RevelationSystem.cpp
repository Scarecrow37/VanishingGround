#include "pchScripts.h"

#include "RevelationSystem.h"

#include "ViewModels/Revelations/RevelationsViewModel.h"

#include <TurnSystem/TurnAction/TurnActionFactory.h>
#include <TurnSystem/TurnMode/TurnMode.h>

#include "ExcelDataSystem/ExcelDataSystem.h"

UMREAL_COMPONENT(RevelationSystem)

using namespace u8_literals;

RevelationSystem::RevelationSystem() 
{
    RevelationsPerRound.SetInputAutoEvent([]() { ImGuiHelper::HoveredToolTip(u8"라운드당 뽑는 계시 개수"); });
}
RevelationSystem::~RevelationSystem()
{
    if (_singletonComponent.IsSingleTon())
    {
        UmWatcher.Unregister<RevelationsViewModel>("Revelations");
    }
};

std::shared_ptr<RevelationElement> RevelationSystem::EquipPlayerElement(int slot, const RevelationElement& element)
{
    std::shared_ptr<RevelationElement> prevElement;
    if (0 <= slot && slot < _playerElementList.size())
    {
        prevElement = std::move(_playerElementList[slot]);
        _playerElementList[slot].reset(new RevelationElement(element));
    }
    return prevElement;
}

std::shared_ptr<RevelationElement> RevelationSystem::RemovePlayerElement(int slot)
{
    std::shared_ptr<RevelationElement> prevElement;
    if (0 <= slot && slot < _playerElementList.size())
    {
        prevElement = std::move(_playerElementList[slot]);
        _playerElementList.erase(_playerElementList.begin() + slot);
    }
    return prevElement;
}

const std::shared_ptr<RevelationElement>& RevelationSystem::PushBackRevelation(const RevelationElement& element)
{
    return _playerElementList.emplace_back(new RevelationElement(element));
}

void RevelationSystem::EquipRandomExtinctionElement(size_t count)
{
    if (count < 1)
        return;

    // 소멸 계시만 필터
    auto& revelations = GetRevelationTableElements();
    std::vector<RevelationElement*> extinctions;
    extinctions.reserve(revelations.size());
    std::ranges::copy_if(revelations, std::back_inserter(extinctions), [](RevelationElement* element)
    { 
        RevelationGrade garde = element->Grade;
        return garde == RevelationGrade::EXTINCTION;
    });

    //랜덤 셔플 후 앞에 2개 추가
    if (false == extinctions.empty())
    {
        auto& engine = Random::GetEngine();
        std::ranges::shuffle(extinctions, engine);
        extinctions.resize(count);

        for (auto& extinction : extinctions)
        {
            if (extinction)
            {
                PushBackRevelation(*extinction);
            }          
        }
    }  
}

void RevelationSystem::RemoveAllExtinctionElements() 
{
    size_t size = _playerElementList.size();
    while (0 < size)
    {
        size_t lastIndex = size - 1;
        auto&  element   = _playerElementList[lastIndex];
        if (nullptr == element)
            break;

        RevelationGrade garde = element->Grade;
        if (garde != RevelationGrade::EXTINCTION)
            break;

        RemovePlayerElement(static_cast<int>(lastIndex));
        size = _playerElementList.size();
    }
}

void RevelationSystem::RollRoundElement()
{
    TurnMode* _turnMode = SingletonComponent<TurnMode>::GetInstance();

    if (_turnMode)
    {
        const auto& roundElementList = _roundElementList;
        //기존 액션들 비활성화
        for (auto& element : roundElementList)
        {
            if (element->IsAction())
            {
                element->GetAction().SetDestroy();
            }
        }
        _roundElementList.clear();

        // 실제 존재하는 계시만 리스트에 넣는다
        for (auto& element : _playerElementList)
        {
            if (element)
            {
                _roundElementList.push_back(element);
            }
        }

        // 랜덤 셔플
        _roundElementList.shuffle(Random::GetEngine());

        // 사용 가능한 개수만 남긴다.
        if (ReflectFields->RevelationsPerRound < _roundElementList.size())
        {
            _roundElementList.resize(ReflectFields->RevelationsPerRound);
        }

        // 뽑힌 횟수 계산 및 액션 활성화
        for (auto& element : roundElementList)
        {
            const std::string& name = element->ElementName;
            _elementTotalAppearances[name]++;
            if (element->IsAction())
            {
                std::weak_ptr<RevelationElement> weakElement = element;
                TurnAction& action = element->GetAction();
                action.OnActionActive = [weakElement]() 
                { 
                    if (auto element = weakElement.lock())
                    {
                        const std::string& name = element->ElementName;
                        std::string msg  = std::format("{}{}", name, (const char*)u8" 발동.");
                        UmLogger.Message(LogLevel::LEVEL_DEBUG, msg);
                    }
                };
                _turnMode->AddTurnAction(&action);
            }
        }
        _totalRollCount += (int)_roundElementList.size();
    }
}

bool RevelationSystem::InsertElement(const RevelationElement& element)
{
    const std::string& key      = element.ElementName;
    bool               result   = false;
    auto               findIter = _elementsTable.find(key);
    if (findIter == _elementsTable.end())
    {
        RevelationElement& myElement = _elementsTable[key];
        myElement                    = element;
        PushElementTableOrderID(myElement);
        result = true;
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"이미 존재하는 Element 이름 입니다.");
    }
    return result;
}

bool RevelationSystem::EraseElement(std::string_view elementName)
{
    bool result   = false;
    auto findIter = _elementsTable.find(elementName.data());
    if (findIter != _elementsTable.end())
    {
        EraseElementTableOrderID(findIter->second);
        _elementsTable.erase(findIter);
        result = true;
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"존재하지 않는 Element 이름 입니다.");
    }
    return result;
}

void RevelationSystem::ClearTable() 
{
    _elementsTable.clear();
    _elementTableOrderID.clear();
}

RevelationElement* RevelationSystem::FindElement(const std::string& elementName)
{
    auto find = _elementsTable.find(elementName);
    if (find != _elementsTable.end())
    {
        return &find->second;
    }
    return nullptr;
}

static ReflectHelper::ImGuiDraw::InputAutoSetting InitSetting()
{
    ReflectHelper::ImGuiDraw::InputAutoSetting setting;
    setting.ShowName = false;
    return setting;
}

void RevelationSystem::ImGuiDrawElementTableEditor() 
{
#ifdef _UMEDITOR
    constexpr const char* TABLE_CLEAR_KEY = (const char*)"clear table";

    if (ImGui::BeginTable("Revelation Stats", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {                      
        static ReflectHelper::ImGuiDraw::InputAutoSetting tableSetting = InitSetting();

        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Grade");
        ImGui::TableSetupColumn("Action");
        ImGui::TableHeadersRow();

        int itemID = 0;
        for (auto& revelation : _elementTableOrderID)
        {
            RevelationElement& element = *revelation;
            const std::string& key     = element.ElementName;

            auto RightClickContext = [&]() 
            {
                if (ImGui::BeginPopupContextItem())
                {
                    if (ImGui::MenuItem("Delete"))
                    {
                        _imguiEvent.DeleteTableBuffer = key;
                        _imguiEvent.OpenDeletePopup   = true;
                    }
                    if (ImGui::MenuItem("Clear Table"))
                    {
                        _imguiEvent.DeleteTableBuffer = TABLE_CLEAR_KEY;
                        _imguiEvent.OpenDeletePopup   = true;
                    }
                    ImGui::EndPopup();
                }
            };

            ImGui::PushStyleColor(ImGuiCol_Text, element.GetGradeColor());
            ImGui::PushID(itemID++);
            {
                ReflectHelper::ImGuiDraw::InputAutoSetting setting = []() 
                {
                    ReflectHelper::ImGuiDraw::InputAutoSetting setting;
                    setting.ShowName = false;
                    return setting;
                }();
                ImGui::TableNextRow();

                //ID
                ImGui::TableSetColumnIndex(0);           
                {
                    if (ReflectHelper::ImGuiDraw::Private::InputAuto(element.RevelationID, setting))
                    {
                        SortElementTableOrderID();
                    }
                    RightClickContext();
                }

                //Name
                ImGui::TableSetColumnIndex(1);            
                {
                    static std::string renameBuffer;
                    const std::string originName = element.ElementName;
                    renameBuffer = originName;
                    bool input = ImGui::InputText("##name", &renameBuffer);
                    if (input)
                    {
                        if (ImGui::IsItemDeactivatedAfterEdit())
                        {
                            RevelationElement tempElement;
                            tempElement = element;
                            tempElement.SetName(renameBuffer);
                            if (renameBuffer != originName)
                            {
                                _imguiEvent.RenameFunc = [this, originName, tempElement]() 
                                {
                                    if (InsertElement(tempElement))
                                    {
                                        EraseElement(originName);
                                    }
                                };
                            }
                        }
                    }
                    RightClickContext();
                }

                //Grade
                ImGui::TableSetColumnIndex(2);
                {
                    ReflectHelper::ImGuiDraw::Private::InputAuto(element.Grade, setting);
                    RightClickContext();
                }

                //Action
                ImGui::TableSetColumnIndex(3);
                {
                    TurnAction::ImGuiDrawActionMaker(key, element._action, element._showActionEditor);
                }           
            }
            ImGui::PopID();
            ImGui::PopStyleColor(1);
        }

        if (_imguiEvent.RenameFunc)
        {
            _imguiEvent.RenameFunc();
            _imguiEvent.RenameFunc = nullptr;
        }

        ImGui::EndTable();
    }

    if (_imguiEvent.OpenDeletePopup)
    {
        _imguiEvent.OpenDeletePopup = false;
        ImGui::OpenPopup("Element Table Delete Modal Popup");
        ImGui::SetNextWindowPos(_tableEditorCenterPos, 0, ImVec2(0.5f, 0.5f));
    }

    if (ImGui::BeginPopupModal("Element Table Delete Modal Popup"))
    {
        ImGui::Text((const char*)u8"이 작업은 되돌릴 수 없습니다.");
        ImGui::Text("%s", _imguiEvent.DeleteTableBuffer.c_str());
        ImGui::SameLine();
        ImGui::Text((const char*)u8"정말 삭제하시겠습니까?");
        ImGui::Separator();
        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            if (_imguiEvent.DeleteTableBuffer != TABLE_CLEAR_KEY)
            {
                EraseElement(_imguiEvent.DeleteTableBuffer);
                ImGui::CloseCurrentPopup();
                _imguiEvent.DeleteTableBuffer = STR_NULL;
            }
            else
            {
                ClearTable();
                ImGui::CloseCurrentPopup();
                _imguiEvent.DeleteTableBuffer = STR_NULL;
            }        
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)) || ImGui::IsKeyReleased(ImGuiKey_Escape))
        {
            _imguiEvent.DeleteTableBuffer = STR_NULL;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    static std::string newElementName;
    ImGui::InputText("##New Element", &newElementName);
    ImGui::SameLine();
    if (ImGui::Button("New Element"))
    {
        if (false == newElementName.empty())
        {
            RevelationElement element;
            element.SetName(newElementName);
            bool result = InsertElement(element);
            if (true == result)
            {
                newElementName.clear();
            }
        }
    }
#endif
}

void RevelationSystem::ImGuiDrawExcelParser()
{
#ifdef _UMEDITOR
    if (ImGui::BeginPopupModal(u8"알림##Dirty Revelation Popup"_c_str))
    {
        auto PopDirtyWeaponElement = [this]() 
        {
            _imguiEvent.ShowDirtyElementPopup = false;
            _imguiEvent.DirtyRevelationElementQueue.pop();
        };

        ImGui::Text(u8"올바르지 않은 형식입니다. 직접 입력해주세요."_c_str);
        RevelationElement& element = *_imguiEvent.DirtyRevelationElementQueue.front();
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
            const std::string& name = element.ElementName;
            EraseElement(name);
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (false == _imguiEvent.DirtyRevelationElementQueue.empty() && false == _imguiEvent.ShowDirtyElementPopup)
    {
        ImGui::OpenPopup(u8"알림##Dirty Revelation Popup"_c_str);
        _imguiEvent.ShowDirtyElementPopup = true;
    }
#endif
}

bool RevelationSystem::ExcelToRevelationElement(RevelationElement& element, const std::string& key, const std::string& data)
{
    if (false == key.empty())
    {
        try
        {
            std::wstring wcharKey = U8ToWString(key);
            if (L"ID" == wcharKey)
            {
                element.RevelationID = std::stoi(data);
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
                
                if (RevelationElement::GetGradeID(RevelationGrade::COMMON) == rarity)
                {
                    element.Grade = RevelationGrade::COMMON;
                }
                else if (RevelationElement::GetGradeID(RevelationGrade::RARE) == rarity)
                {
                    element.Grade = RevelationGrade::RARE;
                }
                else if (RevelationElement::GetGradeID(RevelationGrade::LEGENDARY) == rarity)
                {
                    element.Grade = RevelationGrade::LEGENDARY;
                }
                else if (RevelationElement::GetGradeID(RevelationGrade::EXTINCTION) == rarity)
                {
                    element.Grade = RevelationGrade::EXTINCTION;
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

void RevelationSystem::ActionsToActionDatas() 
{
    ReflectFields->RevelationActionDatas.clear();
    for (auto& [key, element] : _elementsTable)
    {
        if (element.IsAction())
        {
            auto& action = element.GetAction();
            std::string data = action.SerializedReflectFields();
            ReflectFields->RevelationActionDatas[key] = data;
        }
    }
}

void RevelationSystem::ActionDatasToActions()
{
    for (auto& [key, element] : _elementsTable)
    {
        if (element.IsAction())
        {
            auto& action = element.GetAction();
            std::string& data = ReflectFields->RevelationActionDatas[key];
            action.DeserializedReflectFields(data);
        }
    }
}

void RevelationSystem::ElementsToElementDatas() 
{
    ReflectFields->RevelationElementDatas.clear();
    for (auto& [key, element] : _elementsTable)
    {
        std::string data = element.SerializedReflectFields();
        ReflectFields->RevelationElementDatas.emplace_back(data);
    }
}

void RevelationSystem::ElementDatasToElements() 
{
    _elementsTable.clear();
    _elementTableOrderID.clear();
    for (auto& data : ReflectFields->RevelationElementDatas)
    {
        RevelationElement element;
        element.DeserializedReflectFields(data);
        const std::string& key = element.ElementName;
        InsertElement(element);
    }
}

void RevelationSystem::PlayerElementDatasToPlayerElements() 
{
    size_t elementSize = ReflectFields->PlayerElementDatas.size();
    _playerElementList.resize(elementSize);
    for (size_t i = 0; i < elementSize; i++)
    {
        const std::string& data = ReflectFields->PlayerElementDatas[i];
        if (i < _playerElementList.size())
        {
            if (data != STR_NULL)
            {
                auto findIter = _elementsTable.find(data);
                if (findIter != _elementsTable.end())
                {
                    if (_playerElementList[i])
                    {
                        *_playerElementList[i] = findIter->second;
                    }
                    else
                    {
                        _playerElementList[i].reset(new RevelationElement(findIter->second));
                    }                  
                }      
            }   
        }
    }

}

void RevelationSystem::PlayerElementsToPlayerElementDatas() 
{
    ReflectFields->PlayerElementDatas.clear();
    for (auto& playerElement : _playerElementList)
    {
        if (playerElement)
        {
            const std::string& name = playerElement->ElementName;
            ReflectFields->PlayerElementDatas.emplace_back(name);
        }
        else
        {
            ReflectFields->PlayerElementDatas.emplace_back(STR_NULL);
        }
    }
}

void RevelationSystem::SerializedReflectEvent()
{
    ElementsToElementDatas();
    ActionsToActionDatas();
    PlayerElementsToPlayerElementDatas();
}

void RevelationSystem::DeserializedReflectEvent() 
{
    ElementDatasToElements();
    ActionDatasToActions();
    PlayerElementDatasToPlayerElements();
}

void RevelationSystem::ImGuiDrawPropertysEvent() 
{
#ifdef _UMEDITOR
    if (ImGui::Button("Table Editor"))
    {
        _tableEditorOpen = true;
    }

    if (_tableEditorOpen)
    {
        ImGuiViewport* viewPort = ImGui::GetMainViewport();
        ImVec2         center   = viewPort->GetCenter();
        ImVec2         size     = viewPort->Size * 0.75f;
        ImGui::SetNextWindowPos(center, ImGuiCond_Once, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(size, ImGuiCond_FirstUseEver);
        ImGui::Begin("Revelation Table Editor##47D7303C-9D99-4725-8B99-CC5CC6A2C374", &_tableEditorOpen,
                     ImGuiWindowFlags_MenuBar);
        {
            ImVec2 currentWindowPos  = ImGui::GetWindowPos();
            ImVec2 currentWindowSize = ImGui::GetWindowSize();
            _tableEditorCenterPos    = ImVec2(currentWindowPos.x + currentWindowSize.x * 0.5f,
                                              currentWindowPos.y + currentWindowSize.y * 0.5f);

            if (ImGui::BeginMenuBar())
            {
                if (ImGui::MenuItem("Save Table"))
                {
                    std::wstring_view desktopPath = File::GetDesktopPath();
                    File::Path        out;
                    if (File::ShowSaveFileDialog(NULL, L"저장할 경로를 선택하세요.", desktopPath.data(),
                                                 L"RevelationTable.RtTable", {{L"계시 테이블 파일\0", L"*.RtTable\0"}},
                                                 out))
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
                                ofs << SaveElementTable();
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
                                                 {{L"계시 테이블 파일\0", L"*.RtTable\0"}}, false, out))
                    {
                        if (std::filesystem::exists(out.front()))
                        {
                            std::ifstream ifs(out.front());

                            if (ifs.is_open())
                            {
                                std::string content((std::istreambuf_iterator<char>(ifs)),
                                                    std::istreambuf_iterator<char>());
                                LoadElementTable(content);
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
                        std::unordered_set<std::string> validTargets;
                        size_t rowCount = dataBase.RowCount();
                        for (size_t row = 0; row < rowCount; ++row)
                        {
                            RevelationElement temp;
                            bool              result = true;
                            constexpr std::array<std::u8string_view, 3> keyInfos{u8"ID", u8"Name", u8"Rarity"};

                            for (auto& key : keyInfos)
                            {
                                std::string_view data = dataBase.FindData(row, key);
                                result &= ExcelToRevelationElement(temp, (const char*)key.data(), data.data());
                            }

                            const std::string& name = temp.ElementName;
                            if (STR_NULL != name)
                            {
                                auto nameFindIter = _elementsTable.find(name);
                                bool nameFind     = nameFindIter != _elementsTable.end();
                                auto idFindIter   = std::ranges::find_if(_elementTableOrderID, [&temp](RevelationElement* revelation) 
                                {
                                    int tempID = temp.RevelationID;
                                    int currID = revelation->RevelationID;
                                    return tempID == currID;
                                });
                                bool idFind = idFindIter != _elementTableOrderID.end();
                                if (false == nameFind && false == idFind)
                                {
                                    // 없으면 새로 생성
                                    InsertElement(temp);
                                }
                                else if (nameFind)
                                {
                                    // 이미 있으면 데이터만 복사(액션은 유지)
                                    std::string originActionName = nameFindIter->second.ReflectFields->ActionName;
                                    *nameFindIter->second.ReflectFields = *temp.ReflectFields;
                                    nameFindIter->second.ReflectFields->ActionName = std::move(originActionName);
                                }
                                else if (idFind)
                                {
                                    // 이미 있으면 데이터 및 이름 복사(액션은 유지)
                                    RevelationElement* revelation        = *idFindIter;
                                    std::string        originElementName = revelation->ElementName;
                                    std::string        originActionName  = revelation->ReflectFields->ActionName;
                                    temp.ReflectFields->ActionName       = std::move(originActionName);
                                    EraseElement(originElementName);
                                    InsertElement(temp);                                  
                                }
                                if (false == result)
                                {
                                    // 잘못된 데이터는 알림 팝업
                                    RevelationElement& element = _elementsTable[name];
                                    _imguiEvent.DirtyRevelationElementQueue.push(&element);
                                }                               
                                if (auto [iter, insertResult] = validTargets.insert(name); false == insertResult)
                                {           
                                    std::string message = "\"";
                                    message += name;
                                    message += (const char*)u8"\" 는 중복된 계시 이름입니다.";
                                    UmLogger.Log(LogLevel::LEVEL_WARNING, message);
                                }
                            }
                        }     
                        
                        std::vector<std::string> eraseTargets;
                        for (auto& revelation : _elementTableOrderID)
                        {
                            const std::string& name = revelation->ElementName;
                            if (validTargets.find(name) == validTargets.end())
                            {
                                eraseTargets.emplace_back(name);
                            }
                        }
                        for (auto& target : eraseTargets)
                        {
                            EraseElement(target);
                        }
                    };

                    if (ExcelDataSystem* excelSystem = SingletonComponent<ExcelDataSystem>::GetInstance())
                    {
                        std::unique_ptr<ExcelDataBase> dataBase = excelSystem->FindExcelDataBase(u8"계시");
                        if (dataBase)
                        {
                            ParserFunc(*dataBase);
                        }
                    }
                }
                ImGui::EndMenuBar();
            }

            ImGuiDrawElementTableEditor();
            ImGuiDrawExcelParser();
        }
        ImGui::End();
    }

    ImGuiDrawPlayerElementEditor();
    ImGuiDrawRoundElementList();
#endif
}

void RevelationSystem::ImGuiDrawPlayerElementEditor() 
{
    auto TreeToolTip = []() { ImGuiHelper::HoveredToolTip(u8"플레이어가 장착중인 계시 리스트 입니다."); };
    if (ImGui::TreeNodeEx("Player Elements", ImGuiTreeNodeFlags_DefaultOpen))
    {
        TreeToolTip();
        std::shared_ptr<RevelationElement>* eraseSelect = nullptr;
        int eraseSlot = -1;
        int currentSlot = 0;
        for (auto& element : _playerElementList)
        {
            ImGui::PushID(&element);
            std::string_view name = STR_NULL;
            bool elementEmpty = element == nullptr;
            if (false == elementEmpty)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, element->GetGradeColor());
                name = (const std::string&)element->ElementName;
            }
            if (ImGui::BeginCombo("##5794D456-E0A6-4F6C-844B-07D94A6401C6", name.data()))
            {
                for (auto& [key, tableElement] : _elementsTable)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, tableElement.GetGradeColor());
                    if (ImGui::Selectable(key.data()))
                    {
                        if (element)
                        {
                            *element = tableElement;
                        }
                        else
                        {
                            element.reset(new RevelationElement(tableElement));
                        }          
                    }
                    ImGui::PopStyleColor();
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
            if (ImGui::Button("Remove"))
            {
                eraseSelect = &element;
                eraseSlot   = currentSlot;
            }
            if (name != STR_NULL)
            {
                float count = (float)_elementTotalAppearances[name.data()];
                if (count > 0 && _totalRollCount > 0)
                {
                    float percentage = count / (float)_totalRollCount;
                    ImGui::SameLine();
                    ImGui::Text("%f%%", percentage * 100.f);
                }            
            }      
            if (false == elementEmpty)
            {
                ImGui::PopStyleColor();
            }     
            ImGui::PopID();
            currentSlot++;
        }
        if (eraseSelect)
        {
            _roundElementList.erase(*eraseSelect);
            RemovePlayerElement(eraseSlot);
            eraseSelect = nullptr;          
        }
        if (ImGui::Button("Add Element"))
        {
            auto begin = _elementsTable.begin();
            if (begin != _elementsTable.end())
            {
                PushBackRevelation(begin->second);
            }
        }
        ImGui::TreePop();
    }
    else
    {
        TreeToolTip();
    }
}

void RevelationSystem::ImGuiDrawRoundElementList()
{
    auto TreeToolTip = []() { ImGuiHelper::HoveredToolTip(u8"현재 활성화된 계시 항목 입니다."); };
    auto RollButton = [this]() 
    {
        ImGui::SameLine();
        if (ImGui::Button("Roll Round Elements"))
        {
            RollRoundElement();    
        }
        ImGuiHelper::HoveredToolTip(u8"랜덤으로 계시를 뽑습니다.");
    };
    if (ImGui::TreeNodeEx("Round Elements", ImGuiTreeNodeFlags_DefaultOpen))
    {
        RollButton();
        const auto& roundElementList = _roundElementList;
        for (auto& element : roundElementList)
        {
            std::string_view name = (const std::string&)element->ElementName;
            ImGui::PushStyleColor(ImGuiCol_Text, element->GetGradeColor());
            ImGui::Text(name.data());
            ImGui::Separator();
            ImGui::PopStyleColor();
        }
        ImGui::TreePop();
    }
    else
    {
        RollButton();
    }
}

void RevelationSystem::SortElementTableOrderID() 
{
    std::ranges::sort(_elementTableOrderID, [](const RevelationElement* a, const RevelationElement* b) 
    {
        return a->RevelationID < b->RevelationID;
    });
}

void RevelationSystem::PushElementTableOrderID(RevelationElement& element) 
{
    _elementTableOrderID.push_back(&element);
    SortElementTableOrderID();
}

void RevelationSystem::EraseElementTableOrderID(RevelationElement& element) 
{
    std::erase(_elementTableOrderID, &element);
}

void RevelationSystem::Awake()
{
    Component::Awake();
    if (_singletonComponent.TrySingleTon())
    {
        UmWatcher.Register<RevelationsViewModel>("Revelations", _roundElementList);
    }
}

void RevelationSystem::Reset() 
{
    _singletonComponent.SetSingleTon();
}
