#pragma once

namespace NodeGraph
{
    class Pin;
    class Node;
    class Link;

    class Link
    {
    public:
        Link(ed::LinkId id, ed::PinId startPinId, ed::PinId endPinId)
            : ID(id), StartPinID(startPinId), EndPinID(endPinId), Color(255, 255, 255)
        {
        }

    public:
        inline ed::LinkId     GetLinkID() const { return ID; }
        inline ed::PinId      GetStartPinID() const { return StartPinID; }
        inline ed::PinId      GetEndPinID() const { return EndPinID; }
        inline const ImColor& GetColor() const { return Color; }

    private:
        ed::LinkId ID;
        ed::PinId  StartPinID;
        ed::PinId  EndPinID;
        ImColor    Color;
    };
} // namespace NodeGraph