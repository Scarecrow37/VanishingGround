#include "pch.h"
#include "EditorNodeGraph.h"

NodeGraphContext::NodeGraphContext() 
    : _editor(nullptr), _state({}), _uniqueID(0)
{
    ed::Config config;
    config.UserPointer = this;

    config.SaveNodeSettings = [](ed::NodeId nodeId, const char* data, size_t size, ed::SaveReasonFlags reason,
                                 void* userPointer) -> bool {
        auto self = static_cast<NodeGraphContext*>(userPointer);

        auto node = self->FindNodeFromNodeID(nodeId.Get());
        if (!node)
            return false;

        //node->State.assign(data, size);
        //self->TouchNode(nodeId);

        return true;
    };

    //config.SaveSettings = [](const char* data, size_t size, ed::SaveReasonFlags reason, void* userPointer) -> bool
    //{
    //    auto self = static_cast<NodeGraphContext*>(userPointer);
    //    self->SaveData(data, size);
    //
    //    return true;
    //};

    _editor = ed::CreateEditor(&config);
}

NodeGraphContext::~NodeGraphContext() 
{
    Clear();
    ed::DestroyEditor(_editor);
    _editor = nullptr;
}

namespace util = ax::NodeEditor::Utilities;

void NodeGraphContext::Render() 
{
    ImGuiIO&    io    = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ed::SetCurrentEditor(_editor);

    // 그리드 출력
    ImGui::PushID(this);
    ed::Begin("Node editor");
    {
        auto cursorTopLeft = ImGui::GetCursorScreenPos();

        ProcessNodes();
        ProcessLinks();
        ProcessCreate();
        ProcessPopupContext();

        ImGui::SetCursorScreenPos(cursorTopLeft);
    }
    ed::End();
    ImGui::PopID();
}

void NodeGraphContext::Clear() 
{
    for (auto& node : _nodeVector)
    {
        delete node;
    }
    _nodeVector.clear();
    _nodeTable.clear();
    for (auto& link : _linkVector)
    {
        delete link;
    }
    _linkVector.clear();
    _linkTable.clear();
}

NodeGraph::Node* NodeGraphContext::FindNodeFromNodeID(UINT64 nodeID)
{
    auto itr = _nodeTable.find(nodeID);
    if (itr != _nodeTable.end())
        return itr->second;
    return nullptr;
}

NodeGraph::Node* NodeGraphContext::FindNodeFromPinID(UINT64 pinID)
{
    for (auto& node : _nodeVector)
    {
        NodeGraph::Pin* pin = node->FindPin(pinID);
        if (nullptr != pin)
        {
            return node;
        }
    }
    return nullptr;
}

NodeGraph::Pin* NodeGraphContext::FindPinFromPinID(UINT64 pinID)
{
    for (auto& node : _nodeVector)
    {
        NodeGraph::Pin* pin = node->FindPin(pinID);
        if (nullptr != pin)
        {
            return pin;
        }
    }
    return nullptr;
}

void NodeGraphContext::SaveData(const char* data, size_t size) 
{
}

void NodeGraphContext::LoadData(const std::string& data) 
{

}

const char* NodeGraphContext::SaveNodeSettingsToMemory()
{
    return ReflectFields->SerializeData.data();
}

void NodeGraphContext::LoadNodeSettingsFromMemory(const std::string& data) 
{
}

const char* NodeGraphContext::GetNodeSettingsData()
{
    return ReflectFields->SerializeData.data();
}

void NodeGraphContext::SerializedReflectEvent()
{

}

void NodeGraphContext::DeserializedReflectEvent() 
{
}

void NodeGraphContext::ProcessNodes()
{
    for (auto& node : _nodeVector)
    {
        node->Draw();
    }
}

void NodeGraphContext::ProcessLinks() 
{
    for (auto& [id, link] : _linkTable)
    {
        ed::LinkId linkId   = link->GetLinkID();
        ed::PinId  startId  = link->GetStartPinID();
        ed::PinId  endId    = link->GetEndPinID();
        ImColor    pinColor = link->GetPinColor();
        ed::Link(linkId, startId, endId, pinColor, ReflectFields->LinkThickness);
    }
}

void NodeGraphContext::ProcessCreate() 
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

void NodeGraphContext::ProcessCreateLink()
{
    ed::PinId startPinId = 0, endPinId = 0;
    if (ed::QueryNewLink(&startPinId, &endPinId))
    {
        NodeGraph::Pin* startPin = FindPinFromPinID(startPinId.Get());
        NodeGraph::Pin* endPin   = FindPinFromPinID(endPinId.Get());

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
                ImVec4 rejectLinkColor     = ImGuiHelper::FloatPtrToImVec4(ReflectFields->RejectNewLinkData.data());
                float  rejectLinkThickness = ReflectFields->RejectNewLinkData[ReflectFields->THICKNESS];
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
                ImVec4 acceptLinkColor     = ImGuiHelper::FloatPtrToImVec4(ReflectFields->AcceptNewLinkData.data());
                float  acceptLinkThickness = ReflectFields->AcceptNewLinkData[ReflectFields->THICKNESS];
                ShowLabel("+ Create Link", ImColor(32, 45, 32, 180));
                if (ed::AcceptNewItem(acceptLinkColor, acceptLinkThickness))
                {
                    
                    NodeGraph::Link* link = new NodeGraph::Link(startPinId, endPinId, ImColor(255, 255, 255));
                    UINT64 id = link->GetLinkID().Get();
                    _linkVector.push_back(link);
                    _linkTable[id] = link;
                }
            }
        }
    }
}

void NodeGraphContext::ProcessCreateNode() 
{
    ed::PinId pinId = 0;
    if (ed::QueryNewNode(&pinId))
    {
        _state.NewLinkPin = FindPinFromPinID(pinId.Get());
        if (nullptr != _state.NewLinkPin)
        {
            ShowLabel("+ Create Node", ImColor(32, 45, 32, 180));
        }
        if (ed::AcceptNewItem())
        {
            _state.isProcessingNewNode = true;
            _state.NewNodeLinkPin      = FindPinFromPinID(pinId.Get());
            _state.NewLinkPin          = nullptr;
            ed::Suspend();
            ImGui::OpenPopup("Create New Node");
            ed::Resume();
        }
    }
}

void NodeGraphContext::ProcessPopupContext() 
{
    static ed::NodeId contextNodeId = 0;
    static ed::LinkId contextLinkId = 0;
    static ed::PinId  contextPinId  = 0;

    ed::Suspend();
    if (ed::ShowNodeContextMenu(&contextNodeId))
        ImGui::OpenPopup("Node Context Menu");
    else if (ed::ShowPinContextMenu(&contextPinId))
        ImGui::OpenPopup("Pin Context Menu");
    else if (ed::ShowLinkContextMenu(&contextLinkId))
        ImGui::OpenPopup("Link Context Menu");
    else if (ed::ShowBackgroundContextMenu())
    {
        ImGui::OpenPopup("Create New Node");
        _state.NewNodeLinkPin = nullptr;
    }
    ed::Resume();

    ed::Suspend();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

    if (ImGui::BeginPopup("Node Context Menu"))
    {
        auto node = FindNodeFromNodeID(contextNodeId.Get());
        ImGui::TextUnformatted("Node Context Menu");
        ImGui::Separator();
        if (nullptr != node)
        {
            node->OnNodePopup();
        }
        else
            ImGui::Text("Unknown node: %p", contextNodeId.AsPointer());
        ImGui::EndPopup();
    }
    if (ImGui::BeginPopup("Pin Context Menu"))
    {
        auto node = FindNodeFromPinID(contextPinId.Get());
        ImGui::TextUnformatted("Pin Context Menu");
        ImGui::Separator();
        if (nullptr != node)
        {
            node->OnPinPopup(contextPinId.Get());
        }
        else
            ImGui::Text("Unknown pin: %p", contextPinId.AsPointer());
        ImGui::EndPopup();
    }

    ImGui::PopStyleVar();
    ed::Resume();
}

void NodeGraphContext::ShowLabel(const char* label, const ImColor& bgColor, const ImVec4& textColor)
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