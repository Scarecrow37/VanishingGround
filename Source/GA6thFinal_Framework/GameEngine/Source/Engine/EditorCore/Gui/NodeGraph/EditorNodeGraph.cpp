#include "pch.h"
#include "EditorNodeGraph.h"

EditorNodeGraph::EditorNodeGraph() 
    : _editor(nullptr), _state({}), _uniqueID(0)
{
    ed::Config config;
    config.UserPointer = this;

    config.SaveNodeSettings = [](ed::NodeId nodeId, const char* data, size_t size, ed::SaveReasonFlags reason,
                                 void* userPointer) -> bool {
        auto self = static_cast<EditorNodeGraph*>(userPointer);

        auto node = self->FindNode(nodeId);
        if (!node)
            return false;

        //node->State.assign(data, size);
        //self->TouchNode(nodeId);

        return true;
    };

    //config.SaveSettings = [](const char* data, size_t size, ed::SaveReasonFlags reason, void* userPointer) -> bool
    //{
    //    auto self = static_cast<EditorNodeGraph*>(userPointer);
    //    self->SaveData(data, size);
    //
    //    return true;
    //};

    _editor = ed::CreateEditor(&config);
}

EditorNodeGraph::~EditorNodeGraph() 
{
    ed::DestroyEditor(_editor);
    _editor = nullptr;
}

namespace util = ax::NodeEditor::Utilities;

void EditorNodeGraph::Render() 
{
    ImGuiIO&    io    = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ed::SetCurrentEditor(_editor);

    // 그리드 출력
    ImGui::PushID(this);
    ed::Begin("Node editor");
    {
        ProcessNodes();
        ProcessLinkes();
    }
    ed::End();
    ImGui::PopID();
}

NodeGraph::Node* EditorNodeGraph::FindNode(ed::NodeId id)
{
    auto itr = _nodeTable.find(id.Get());
    if (itr != _nodeTable.end())
        return itr->second;
    return nullptr;
}

NodeGraph::Link* EditorNodeGraph::FindLink(ed::LinkId id)
{
    auto itr = _linkTable.find(id.Get());
    if (itr != _linkTable.end())
        return itr->second;
    return nullptr;
}

NodeGraph::Pin* EditorNodeGraph::FindPin(ed::PinId id)
{
    for (auto& node : _nodeVector)
    {
        NodeGraph::Pin* pin = node->FindPin(id);
        if (nullptr != pin)
        {
            return pin;
        }
    }
    return nullptr;
}

void EditorNodeGraph::SaveData(const char* data, size_t size) 
{
}

void EditorNodeGraph::LoadData(const std::string& data) 
{

}

const char* EditorNodeGraph::SaveNodeSettingsToMemory()
{
    return ReflectFields->SerializeData.data();
}

void EditorNodeGraph::LoadNodeSettingsFromMemory(const std::string& data) 
{
}

const char* EditorNodeGraph::GetNodeSettingsData()
{
    return ReflectFields->SerializeData.data();
}

NodeGraph::Link* EditorNodeGraph::AddLink(ed::PinId startPinId, ed::PinId endPinId, const ImColor& pinColor)
{
    UINT64 id = GetUniqueID();
    NodeGraph::Link* link = new NodeGraph::Link(id, startPinId, endPinId, pinColor);
    _linkVector.push_back(link);
    _linkTable[id] = link;
    return link;
}

void EditorNodeGraph::SerializedReflectEvent()
{
    ReflectFields->SerializeData = ed::SaveIniSettingsToMemory();
}

void EditorNodeGraph::DeserializedReflectEvent() 
{
}

void EditorNodeGraph::ProcessNodes()
{
    for (auto& node : _nodeVector)
    {
        node->Draw();
    }
}

void EditorNodeGraph::ProcessLinkes() 
{
    for (auto& link : _linkVector)
    {
        ed::LinkId linkId   = link->GetLinkID();
        ed::PinId  startId  = link->GetStartPinID();
        ed::PinId  endId    = link->GetEndPinID();
        ImColor    pinColor = link->GetPinColor();
        ed::Link(linkId, startId, endId, pinColor, ReflectFields->LinkThickness);
    }
}

void EditorNodeGraph::ProcessCreate() 
{
    ImVec4 defaultNewLinkColor = ImGuiHelper::FloatPtrToImVec4(ReflectFields->DefaultNewLinkData.data());
    float  defaultNewLinkThickness = ReflectFields->DefaultNewLinkData[ReflectFields->THICKNESS];

    if (ed::BeginCreate(defaultNewLinkColor, defaultNewLinkThickness))
    {
        ProcessCreateLink();
        ProcessCreateNode();
        ed::EndCreate();
    }
}

void EditorNodeGraph::ProcessCreateLink() 
{
    ed::PinId startPinId = 0, endPinId = 0;
    if (ed::QueryNewLink(&startPinId, &endPinId))
    {
        auto startPin = FindPin(startPinId);
        auto endPin   = FindPin(endPinId);

        _state.NewLinkPin = startPin ? startPin : endPin;

        if (nullptr != startPin && nullptr != endPin)
        {
            if (ed::PinKind::Input == startPin->GetPinKind())
            {
                std::swap(startPin, endPin);
                std::swap(startPinId, endPinId);
            }
            
            bool isSamePin = endPin == startPin;
            bool isSameKind = endPin->GetPinKind() == startPin->GetPinKind();
            bool isSameType = endPin->GetPinType() == startPin->GetPinType();
            // 같은 핀이거나, 같은 종류(입력, 출력)의 핀이고, 같은 타입의 핀이 아닌 경우는 연결할 수 없습니다.
            bool isReject = false == isSamePin && true == isSameKind && false == isSameType;
            //////////////////////////////////////
            // 연결할 수 없는 핀 처리
            //////////////////////////////////////
            if (true == isReject)
            {
                ImVec4 rejectLinkColor     = ImGuiHelper::FloatPtrToImVec4(ReflectFields->DefaultNewLinkData.data());
                float  rejectLinkThickness = ReflectFields->DefaultNewLinkData[ReflectFields->THICKNESS];
                if (true == isSameKind)
                {   // 입력인 경우 출력에, 출력인 경우 입력에만 허용
                    ShowLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
                }
                else if (false == isSameType)
                {   // 종류가 다른 핀일 경우
                    ShowLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
                }
                ed::RejectNewItem(rejectLinkColor, rejectLinkThickness);
            }
            //////////////////////////////////////
            // 연결 가능한 핀 처리
            //////////////////////////////////////
            else
            {
                ImVec4 acceptLinkColor     = ImGuiHelper::FloatPtrToImVec4(ReflectFields->DefaultNewLinkData.data());
                float  acceptLinkThickness = ReflectFields->DefaultNewLinkData[ReflectFields->THICKNESS];
                ShowLabel("+ Create Link", ImColor(32, 45, 32, 180));
                if (ed::AcceptNewItem(acceptLinkColor, acceptLinkThickness))
                {
                    AddLink(startPinId, endPinId);
                }
            }
        }
    }
}

void EditorNodeGraph::ProcessCreateNode() 
{
    ed::PinId pinId = 0;
    if (ed::QueryNewNode(&pinId))
    {
        _state.NewLinkPin = FindPin(pinId);
        if (nullptr != _state.NewLinkPin)
            ShowLabel("+ Create Node", ImColor(32, 45, 32, 180));

        if (ed::AcceptNewItem())
        {
            _state.isProcessingNewNode = true;
            _state.NewNodeLinkPin      = FindPin(pinId);
            _state.NewLinkPin          = nullptr;
            ed::Suspend();
            ImGui::OpenPopup("Create New Node");
            ed::Resume();
        }
    }
}

void EditorNodeGraph::ShowLabel(const char* label, const ImColor& bgColor, const ImVec4& textColor)
{
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
    auto size = ImGui::CalcTextSize(label);

    auto padding = ImGui::GetStyle().FramePadding;
    auto spacing = ImGui::GetStyle().ItemSpacing;

    ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

    auto rectMin = ImGui::GetCursorScreenPos() - padding;
    auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

    auto drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(rectMin, rectMax, bgColor, size.y * 0.15f);

    ImGui::PushStyleColor(ImGuiCol_Text, textColor);
    ImGui::TextUnformatted(label);
    ImGui::PopStyleColor();
}
