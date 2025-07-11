#include "pchScripts.h"
#include "RevelationSystem.h"
RevelationSystem::RevelationSystem() = default;
RevelationSystem::~RevelationSystem() = default;

bool RevelationSystem::InsertElement(const RevelationElement& element) 
{
    std::string_view key      = element.Name;
    bool             result   = false;
    auto             findIter = _elements.find(key.data());
    if (findIter == _elements.end())
    {
        RevelationElement& myElement = _elements[key.data()];
        myElement                    = element;
        result                       = true;
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
    auto findIter = _elements.find(elementName.data());
    if (findIter != _elements.end())
    {
        _elements.erase(findIter);
        result = true;
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"존재하지 않는 Element 이름 입니다.");
    }
    return result;
}

void RevelationSystem::DrawImGuiElementTableEditor() 
{
    if (ImGui::BeginTable("Revelation Stats", 8, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {                      
        ImGui::TableSetupColumn("ImageGuid");
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Condition");
        ImGui::TableSetupColumn("ConditionValueA");
        ImGui::TableSetupColumn("ConditionValueB");
        ImGui::TableSetupColumn("Keyword");
        ImGui::TableSetupColumn("Grade");
        ImGui::TableSetupColumn("Action");
        ImGui::TableHeadersRow();

        for (auto& [key, element] : _elements)
        {
            auto RightClickContext = [&]() {
                if (ImGui::BeginPopupContextItem())
                {
                    if (ImGui::MenuItem("Rename"))
                    {
                        _imguiEvent.RenameBuffer    = key;
                        _imguiEvent.SelectElement   = &element;
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
            ImGui::PushStyleColor(ImGuiCol_Text, element.GetGradeColor());
            ImGui::PushID(itemID++);
            {
                ImGui::TableNextRow();
                element.SetImGuiTableIndex();
                UmCore->ImGuiDrawPropertysSetting.InputEndEvent = [&](bool edit, std::string_view name) 
                {
                    element.SetImGuiTableIndex();
                    RightClickContext();
                };
                element.ImGuiDrawPropertys();
            }
            ImGui::PopID();
            ImGui::PopStyleColor(1);
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
            EraseElement(_imguiEvent.DeleteTableBuffer);
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

    if (_imguiEvent.OpenRenamePopup)
    {
        _imguiEvent.OpenRenamePopup = false;
        ImGui::OpenPopup("Element Table Rename Popup");
    }

    if (ImGui::BeginPopup("Element Table Rename Popup"))
    {
        ImGui::InputText("##Rename", &_imguiEvent.RenameBuffer);
        ImGui::SameLine();
        if (ImGui::Button("Rename"))
        {
            if (false == _imguiEvent.RenameBuffer.empty())
            {
                RevelationElement element;
                element = *_imguiEvent.SelectElement;
                element.SetName(_imguiEvent.RenameBuffer);
                if (InsertElement(element))
                {
                    std::string_view key = _imguiEvent.SelectElement->Name;
                    EraseElement(key);
                }
                _imguiEvent.SelectElement = nullptr;
                _imguiEvent.RenameBuffer = STR_NULL;
                ImGui::CloseCurrentPopup();
            }
        }
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            _imguiEvent.SelectElement = nullptr;
            _imguiEvent.RenameBuffer = STR_NULL;
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
}

void RevelationSystem::ActionsToActionDatas() 
{
    ReflectFields->RevelationActionDatas.clear();
    for (auto& [key, element] : _elements)
    {
        if (auto action = element.GetAction())
        {
            std::string data = action->SerializedReflectFields();
            ReflectFields->RevelationActionDatas[key] = data;
        }
    }
}

void RevelationSystem::ActionDatasToActions()
{
    for (auto& [key, element] : _elements)
    {
        if (auto action = element.GetAction())
        {
            std::string data = ReflectFields->RevelationActionDatas[key];
            action->DeserializedReflectFields(data);
        }
    }
}

void RevelationSystem::ElementsToElementDatas() 
{
    ReflectFields->RevelationElementDatas.clear();
    for (auto& [key, element] : _elements)
    {
        std::string data = element.SerializedReflectFields();
        ReflectFields->RevelationElementDatas.emplace_back(data);
    }
}

void RevelationSystem::ElementDatasToElements() 
{
    _elements.clear();
    for (auto& data : ReflectFields->RevelationElementDatas)
    {
        RevelationElement element;
        element.DeserializedReflectFields(data);
        std::string_view key = element.Name;
        InsertElement(element);
    }
}

void RevelationSystem::SerializedReflectEvent()
{
    ElementsToElementDatas();
    ActionsToActionDatas();
}

void RevelationSystem::DeserializedReflectEvent() 
{
    ElementDatasToElements();
    ActionDatasToActions();
}

void RevelationSystem::ImGuiDrawPropertysEvent() 
{
    if(ImGui::Button("Table Editor"))
    {
        _tableEditorOpen = true;
    }

    if (_tableEditorOpen)
    {
        ImGuiViewport* viewPort = ImGui::GetMainViewport();
        ImVec2         center   = viewPort->GetCenter();
        ImVec2         size     = viewPort->Size;
        ImGui::SetNextWindowPos(center, ImGuiCond_Once, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(size, ImGuiCond_FirstUseEver);
        ImGui::Begin("Revelation Table Editor##93BDE7E1-62AF-49F4-8FCD-3115FC907146", &_tableEditorOpen,
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
                ImGui::EndMenuBar();
            }

            DrawImGuiElementTableEditor();
        }
        ImGui::End();
    }
}

void RevelationSystem::Reset()
{
    static_instance = this;
    ResetActions();
}

void RevelationSystem::ResetActions() 
{
    _actionConstructors.clear();
    std::unique_ptr<RevelationActionBase> actionTemp;

    const auto& construtors = GetInstanceConstructors();
    for (auto& [key, func] : construtors)
    {
        actionTemp.reset(func());
        std::string_view name = actionTemp->Name;
        auto             findIter = construtors.find(name.data());
        if (findIter != construtors.end())
        {
            std::string message = std::format("{} {}", (const char*)u8"이름이 같은 Revelation Action이 존재합니다.", typeid(*actionTemp).name());
            UmLogger.Log(LogLevel::LEVEL_WARNING, message);
        }
        _actionConstructors[name.data()] = func;
    }
}
