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
        /// <summary>
        /// 플로우 애니메이션을 재생합니다.
        /// </summary>
        /// <param name="reverse">플로우 방향을 반전시킬지 여부입니다</param>
        void SetFlow(bool reverse = false);

        inline UINT64 GetLinkID() const { return _id; }
        inline UINT64 GetStartPinID() const { return _startPinID; }
        inline UINT64 GetEndPinID() const { return _endPinID; }
        inline const ImColor& GetPinColor() const { return _color; }

    private:
        UINT64  _id;
        UINT64  _startPinID;
        UINT64  _endPinID;
        ImColor _color;
    };
} // namespace NodeGraph