#include "pch.h"
#include "NodeGraphNode.h"

namespace NodeGraph
{
    Node::Node()
        : _id(GetUniqueID())
    {
    }
    Node::~Node()
    {
        _inputPinList.clear();
        _outputPinList.clear();
        _pinIDTable.clear();
        _pinNameTable.clear();
    }

    Pin* Node::FindPin(UINT64 id)
    {
        auto it = _pinIDTable.find(id);
        if (it != _pinIDTable.end())
        {
            return it->second;
        }
        return nullptr;
    }
    Pin* Node::FindPin(const char* label)
    {
        auto it = _pinNameTable.find(label);
        if (it != _pinNameTable.end())
        {
            return it->second;
        }
        return nullptr;
    }
    Pin* Node::AddPin(const char* label, const char* type, ed::PinKind kind)
    {
        Pin* pin = nullptr;
        SetCurrentNode(this);
        if (kind == ed::PinKind::Input)
        {
            _inputPinList.emplace_back(label, type, kind);
            pin = &_inputPinList.back();        }
        else if (kind == ed::PinKind::Output)
        {
            _outputPinList.emplace_back(label, type, kind);
            pin = &_outputPinList.back();
        }
        if (nullptr != pin)
        {
            UINT64 id = pin->GetPinID();
            _pinIDTable[id] = pin;
            _pinNameTable[label] = pin;
        }
        return pin;
    }
    bool Node::RemovePin(UINT64 id)
    {
        return RemovePin(FindPin(id));
    }
    bool Node::RemovePin(const char* label)
    {
        return RemovePin(FindPin(label));
    }
    bool Node::RemovePin(Pin* pin)
    {
        if (nullptr == pin)
        {
            return false;
        }
        auto idIt = _pinIDTable.find(pin->GetPinID());
        if (idIt != _pinIDTable.end())
        {
            _pinIDTable.erase(idIt);
        }
        auto nameIt = _pinNameTable.find(pin->GetPinLabel());
        if (nameIt != _pinNameTable.end())
        {
            _pinNameTable.erase(nameIt);
        }
        ed::PinKind kind = pin->GetPinKind();
        if (ed::PinKind::Input == kind)
        {
            for (auto it = _inputPinList.begin(); it != _inputPinList.end(); ++it)
            {
                if (it->GetPinID() == pin->GetPinID())
                {
                    _inputPinList.erase(it);
                    break;
                }
            }
        }
        else if (ed::PinKind::Output == kind)
        {
            for (auto it = _outputPinList.begin(); it != _outputPinList.end(); ++it)
            {
                if (it->GetPinID() == pin->GetPinID())
                {
                    _outputPinList.erase(it);
                    break;
                }
            }
        }
        return false;
    }
    void Node::SetPosition(const ImVec2& pos) 
    {
        ed::SetNodePosition(GetNodeID(), pos);
    }
    void Node::SetSeletion(bool append) 
    {
        ed::SelectNode(GetNodeID(), append);
    }
    void Node::SetZOrder(float zOrder) 
    {
        ed::SetNodeZPosition(GetNodeID(), zOrder);
    }
    const Pin* Node::GetInputPin(size_t index)
    {
        if (GetInputPinCount() - 1 > index)
        {
            return nullptr;
        }
        return &_inputPinList[index];
    }
    const Pin* Node::GetOutputPin(size_t index)
    {
        if (GetOutputPinCount() - 1 > index)
        {
            return nullptr;
        }
        return &_outputPinList[index];
    }
} // namespace NodeGraph