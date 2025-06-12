#include "pch.h"
#include "NodeGraphNode.h"

namespace NodeGraph
{
    Node::Node(const char* label)
        : _id(GetUniqueID()), _label(label), _color(ImColor(255, 255, 255)), _size(0, 0)
    {
    }
    Node::~Node()
    {
        _inputPinList.clear();
        _outputPinList.clear();
        _totalPinList.clear();
        _pinTable.clear();
    }

    Pin* Node::FindPin(ed::PinId id)
    {
        auto it = _pinTable.find(id.Get());
        if (it != _pinTable.end())
        {
            return it->second;
        }
        return nullptr;
    }

    Pin* Node::AddPin(const char* name, const char* type, ed::PinKind kind)
    {
        Pin* pin = nullptr;
        UINT64 id;
        SetCurrentNode(this);
        if (kind == ed::PinKind::Input)
        {
            _inputPinList.emplace_back(name, type, kind);
            pin = &_inputPinList.back();
            id  = pin->GetPinID().Get();
        }
        else if (kind == ed::PinKind::Output)
        {
            _outputPinList.emplace_back(name, type, kind);
            pin = &_outputPinList.back();
            id  = pin->GetPinID().Get();
        }

        if (nullptr != pin)
        {
            _totalPinList.push_back(pin);
            _pinTable[id] = pin;
        }
        return pin;
    }

    void Node::SetPosition(const ImVec2& pos) 
    {
        ed::SetNodePosition(_id, pos);
    }
} // namespace NodeGraph