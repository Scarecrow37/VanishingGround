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
} // namespace NodeGraph