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
    void Link::SetFlow(bool reverse) 
    {
        if (false == reverse)
        {
            ed::Flow(GetLinkID(), ed::FlowDirection::Forward);
        }
        else
        {
            ed::Flow(GetLinkID(), ed::FlowDirection::Backward);
        }
    }
} // namespace NodeGraph