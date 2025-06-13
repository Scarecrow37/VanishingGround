#include "pch.h"
#include "NodeGraphLink.h"

namespace NodeGraph
{
    Link::Link(ed::PinId startPinId, ed::PinId endPinId, ImColor pinColor)
        : _id(GetUniqueID())
        , _startPinID(startPinId)
        , _endPinID(endPinId)
        , _color(pinColor)
    {
    }
    Link::~Link() 
    {
        ed::DeleteLink(GetLinkID());
    }
} // namespace NodeGraph