#include "pch.h"
#include "NodeGraphPin.h"

namespace NodeGraph
{
    Pin::Pin(const char* name, const char* type, ed::PinKind kind)
        : _id(NodeGraph::GetUniqueID())
        , _ownerNodeID(NodeGraph::GetCurrentNode()->GetNodeID())
        , _label(name)
        , _type(type)
        , _kind(kind) 
    {
    }
    Pin::~Pin() 
    {
    }

    /* Static */
    bool Pin::IsSameOwner(const Pin* a, const Pin* b)
    {
        return a->GetOwnerNodeID() == b->GetOwnerNodeID();
    }
    bool Pin::IsSameKind(const Pin* a, const Pin* b)
    {
        return a->GetPinKind() == b->GetPinKind();
    }
    bool Pin::IsSameType(const Pin* a, const Pin* b)
    {
        return strcmp(a->GetPinType(), b->GetPinType()) == 0;
    }
    bool Pin::IsSamePin(const Pin* a, const Pin* b)
    {
        return a == b;
    }
} // namespace NodeGraph