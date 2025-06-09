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

    Pin* Node::FindPin(ed::PinId id)
    {
        for (auto& pin : _inputPinList)
        {
            if (pin.GetPinID() == id)
                return &pin;
        }
        for (auto& pin : _outputPinList)
        {
            if (pin.GetPinID() == id)
                return &pin;
        }
        return nullptr;
    }

    void Node::AddInputPin(const char* name, PinType type)
    {
        UINT64 id = _owner->GetUniqueID();
        _inputPinList.emplace_back(id, name, type, ed::PinKind::Input, this);
        _pinTable[id] = &_inputPinList.back();
    }

    void Node::AddOutputPin(const char* name, PinType type)
    {
        UINT64 id = _owner->GetUniqueID();
        _outputPinList.emplace_back(id, name, type, ed::PinKind::Output, this);
        _pinTable[id] = &_outputPinList.back();
    }
    void Node::SetPosition(const ImVec2& pos) 
    {
        _owner->SetCurrentContext();
        ed::SetNodePosition(_id, pos);
    }
} // namespace NodeGraph