#pragma once

namespace NodeGraph
{
    class Pin;
    class Node;
    class Link;

    class Link
    {
    public:
        Link(ed::LinkId id, ed::PinId startPinId, ed::PinId endPinId, ImColor pinColor)
            : _id(id), _startPinID(startPinId), _endPinID(endPinId), _color(255, 255, 255)
        {
        }

    public:
        inline ed::LinkId     GetLinkID() const { return _id; }
        inline ed::PinId      GetStartPinID() const { return _startPinID; }
        inline ed::PinId      GetEndPinID() const { return _endPinID; }
        inline const ImColor& GetPinColor() const { return _color; }

    private:
        ed::LinkId _id;
        ed::PinId  _startPinID;
        ed::PinId  _endPinID;
        ImColor    _color;
    };
} // namespace NodeGraph