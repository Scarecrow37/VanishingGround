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

        NodeGraph::Node* node;

        if (false == self->FindNodeFromNodeID(nodeId.Get(), &node))
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
        ProcessRemove();
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

bool NodeGraphContext::FindNodeFromNodeID(IN UINT64 nodeID, OUT NodeGraph::Node** ppNode)
{
    auto itr = _nodeTable.find(nodeID);
    if (itr != _nodeTable.end())
    {
        (*ppNode) = itr->second;
        return true;
    }
    return false;
}

bool NodeGraphContext::FindNodeFromPinID(IN UINT64 pinID, OUT NodeGraph::Node** ppNode)
{
    for (auto& node : _nodeVector)
    {
        NodeGraph::Pin* pin = node->FindPin(pinID);
        if (nullptr != pin)
        {
            (*ppNode) = node;
            return true;
        }
    }
    return false;
}

bool NodeGraphContext::FindPinFromPinID(IN UINT64 pinID, OUT NodeGraph::Pin** ppPin)
{
    for (auto& node : _nodeVector)
    {
        NodeGraph::Pin* pin = node->FindPin(pinID);
        if (nullptr != pin)
        {
            (*ppPin) = pin;
            return true;
        }
    }
    return false;
}

bool NodeGraphContext::FindLinkFromLinkID(IN UINT64 linkID, OUT NodeGraph::Link** ppLink)
{
    auto itr = _linkTable.find(linkID);
    if (itr != _linkTable.end())
    {
        (*ppLink) = itr->second;
        return true;
    }
    return false;
}

bool NodeGraphContext::FindPinsFromLinkID(IN UINT64 linkID, OUT NodeGraph::Pin** ppStartPin,OUT NodeGraph::Pin** ppEndPin)
{
    ed::PinId startID, endID;
    if (true == ed::GetLinkPins(linkID, &startID, &endID))
    {
        FindPinFromPinID(startID.Get(), ppStartPin);
        FindPinFromPinID(endID.Get(), ppEndPin);
        return true;
    }
    return false;
}

bool NodeGraphContext::RemoveNodeFromNodeID(IN UINT64 nodeID)
{
    for (auto it = _nodeVector.begin(); it != _nodeVector.end(); ++it)
    {
        if ((*it)->GetNodeID() == nodeID)
        {
            delete *it;
            _nodeVector.erase(it);
            _nodeTable.erase(nodeID);
            ed::DeleteNode(nodeID);
            return true;
        }
    }
    return false;
}

bool NodeGraphContext::RemoveLinkFromLinkID(IN UINT64 linkID)
{
    for (auto it = _linkVector.begin(); it != _linkVector.end(); ++it)
    {
        if ((*it)->GetLinkID().Get() == linkID)
        {
            delete *it;
            _linkVector.erase(it);
            _linkTable.erase(linkID);
            ed::DeleteLink(linkID);
            return true;
        }
    }
    return false;
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
        NodeGraph::Pin* startPin = nullptr;
        NodeGraph::Pin* endPin   = nullptr;

        FindPinFromPinID(startPinId.Get(), &startPin);
        FindPinFromPinID(endPinId.Get(), &endPin);

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
        if (true == FindPinFromPinID(pinId.Get(), &_state.NewLinkPin))
        {
            ShowLabel("+ Create Node", ImColor(32, 45, 32, 180));
        }
        if (ed::AcceptNewItem())
        {
            _state.isProcessingNewNode = true;
            _state.NewNodeLinkPin      = _state.NewLinkPin;
            _state.NewLinkPin          = nullptr;
            ed::Suspend();
            ImGui::OpenPopup("Create New Node");
            ed::Resume();
        }
    }
}

void NodeGraphContext::ProcessRemove() 
{
    if (ed::BeginDelete())
    {
        ProcessRemoveNode();
        ProcessRemoveLink();
        ed::EndDelete();
    }
}

void NodeGraphContext::ProcessRemoveNode() 
{
    ed::NodeId nodeId = 0;
    while (ed::QueryDeletedNode(&nodeId))
    {
        if (ed::AcceptDeletedItem())
        {
            RemoveNodeFromNodeID(nodeId.Get());
        }
    }
}

void NodeGraphContext::ProcessRemoveLink() 
{
    ed::LinkId linkId = 0;
    while (ed::QueryDeletedLink(&linkId))
    {
        if (ed::AcceptDeletedItem())
        {
           RemoveLinkFromLinkID(linkId.Get());
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
        NodeGraph::Node* node = nullptr;
        if (true == FindNodeFromNodeID(contextNodeId.Get(), &node))
        {
            ImGui::TextUnformatted("Node Context Menu");
            ImGui::Separator();
            node->OnNodePopup();
        }
        else
        {
            ImGui::Text("Unknown node: %p", contextNodeId.AsPointer());
        }
        ImGui::EndPopup();
    }
    if (ImGui::BeginPopup("Pin Context Menu"))
    {
        NodeGraph::Node* node = nullptr;
        if (true == FindNodeFromPinID(contextPinId.Get(), &node))
        {
            ImGui::TextUnformatted("Pin Context Menu");
            ImGui::Separator();
            node->OnPinPopup(contextPinId.Get());
        }
        else
        {
            ImGui::Text("Unknown pin: %p", contextPinId.AsPointer());
        }  
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