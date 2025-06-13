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
    void Link::SetFlow(bool reverse, float duration, float flowSpeed) 
    {
        auto& style = ed::GetStyle();
        float oldDuration  = style.FlowDuration;
        float oldSpeed     = style.FlowSpeed;
        style.FlowSpeed    = flowSpeed <= 0.0f ? style.FlowSpeed : flowSpeed;
        style.FlowDuration = duration <= 0.0f ? style.FlowDuration : duration;
        if (false == reverse)
        {
            ed::Flow(GetLinkID(), ed::FlowDirection::Forward);
        }
        else
        {
            ed::Flow(GetLinkID(), ed::FlowDirection::Backward);
        }
        style.FlowSpeed    = oldSpeed;
        style.FlowDuration = oldDuration;
    }
    Pin* Link::GetStartPin() const
    {
        auto context = GetCurrentNodeGraphContext();
        Pin* pin;
        context->FindPinFromPinID(GetStartPinID(), &pin);
        return pin;
    }
    Pin* Link::GetEndPin() const
    {
        auto context = GetCurrentNodeGraphContext();
        Pin* pin;
        context->FindPinFromPinID(GetEndPinID(), &pin);
        return pin;
    }
} // namespace NodeGraph