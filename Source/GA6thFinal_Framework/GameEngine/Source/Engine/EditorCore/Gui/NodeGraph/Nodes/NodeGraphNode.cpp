#include "pch.h"
#include "NodeGraphNode.h"

namespace NodeGraph
{
    Node::Node(EditorNodeGraph* owner, const char* name)
        : _owner(owner), _id(owner->GetUniqueID()), _name(name), _color(ImColor(255, 255, 255)), _size(0, 0)
    {
    }
    Node::~Node()
    {
        _inputPinList.clear();
        _outputPinList.clear();
    }

    void Node::AddInputPin(const char* name, PinType type)
    {
        _inputPinList.emplace_back(_owner->GetUniqueID(), name, type, ed::PinKind::Input, this);
    }

    void Node::AddOutputPin(const char* name, PinType type)
    {
        _outputPinList.emplace_back(_owner->GetUniqueID(), name, type, ed::PinKind::Output, this);
    }
    void Node::SetPosition(const ImVec2& pos) 
    {
        _owner->SetCurrentContext();
        ed::SetNodePosition(_id, pos);
    }
} // namespace NodeGraph