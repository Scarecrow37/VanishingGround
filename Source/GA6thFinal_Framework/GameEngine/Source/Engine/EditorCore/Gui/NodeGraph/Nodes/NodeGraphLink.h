#pragma once

namespace NodeGraph
{
    class Pin;
    class Node;
    class Link;

    class Link
    {
    public:
        Link(ed::PinId startPinId, ed::PinId endPinId, ImColor pinColor = ImColor(255, 255, 255, 255));
        ~Link();

    public:
        inline ed::LinkId     GetLinkID() const { return _id; }
        inline ed::PinId      GetStartPinID() const { return _startPinID; }
        inline ed::PinId      GetEndPinID() const { return _endPinID; }
        inline const ImColor& GetPinColor() const { return _color; }

    private:
        UINT64  _id;
        UINT64  _startPinID;
        UINT64  _endPinID;
        ImColor _color;
    };
} // namespace NodeGraph