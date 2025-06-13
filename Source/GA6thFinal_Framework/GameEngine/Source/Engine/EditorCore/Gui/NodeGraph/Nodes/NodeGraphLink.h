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
        /// <param name="duration">플로우 애니메이션 재생 시간입니다(0이하일 시 기본 값 적용)</param>
        /// <param name="flowSpeed">플로우 애니메이션 재생 속도입니다(0이하일 시 기본 값 적용)</param>
        void SetFlow(bool reverse = false, float duration = -1.0f, float flowSpeed  = -1.0f);

        /// <summary>
        /// 시작 핀의 포인터를 가져옵니다.
        /// </summary>
        /// <returns>시작 핀의 포인터</returns>
        Pin* GetStartPin() const;

        /// <summary>
        /// 끝 핀의 포인터를 가져옵니다.
        /// </summary>
        /// <returns>끝 핀의 포인터</returns>
        Pin* GetEndPin() const;

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