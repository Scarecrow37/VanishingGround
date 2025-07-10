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
    if (ImGui::BeginTable("Weapon Stats", 8, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
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
    for (auto& [key, action] : _actions)
    {
        std::string data = action->SerializedReflectFields();
        ReflectFields->RevelationActionDatas.emplace_back(key, data);
    }
}

void RevelationSystem::ActionDatasToActions() 
{
    _actions.clear();
    for (int i = 0; i < ReflectFields->RevelationActionDatas.size(); ++i)
    {
        auto& [key, data]            = ReflectFields->RevelationActionDatas[i];
        RevelationActionBase* action = _actions[key].get();
        action->DeserializedReflectFields(data);
    }
}

void RevelationSystem::ElementsToElementDatas() 
{
    ReflectFields->RevelationElementDatas.clear();
    for (auto& [key, element] : _elements)
    {
        std::string data = element.SerializedReflectFields();
        ReflectFields->RevelationElementDatas.emplace_back(key, data);
    }
}

void RevelationSystem::ElementDatasToElements() 
{
    _elements.clear();
    for (auto& [key, data] : ReflectFields->RevelationElementDatas)
    {
        _elements[key].DeserializedReflectFields(data);
    }
}

void RevelationSystem::SerializedReflectEvent()
{
    ActionsToActionDatas();
    ElementsToElementDatas();
}

void RevelationSystem::DeserializedReflectEvent() 
{
    ActionDatasToActions();
    ElementDatasToElements();
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
            DrawImGuiElementTableEditor();
        }
        ImGui::End();
    }
}

void RevelationSystem::Reset()
{
    static_instance = this;

    _actions.clear();
    const auto& construtors = GetInstanceConstructors();
    for (auto& [key, func] : construtors)
    {
        _actions[key].reset(func());
    }
}
