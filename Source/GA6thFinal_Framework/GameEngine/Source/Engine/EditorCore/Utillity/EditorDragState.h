#pragma once

class EditorDragState
{
public:
    using State = int;
    enum EState
    {
        DRAG_STATE_NONE = 0,
        DRAG_STATE_START,
        DRAG_STATE_DRAGGING,
        DRAG_STATE_END
    };
    enum EFlags
    {
        DRAG_FLAG_NONE  = 0,
        DRAG_FLAG_LOCK  = 1 << 0, // 다른 드래그 상태가 존재하면 드래그 상태를 트리거하지 않습니다.
        DRAG_FLAG_MOVED = 1 << 1  // 마우스가 움직일 때만 드래그 상태가 트리거 됩니다.
    };

public:
    EditorDragState();
    ~EditorDragState();

public:
    void    ClearDragState();
    void    SetDragState(UINT id, State state);
    void    SetDragState(const char* id, State state);
    State   BeginDragState(UINT id, const ImRect& dragRect, const ImVec2& mousePos,
                           ImGuiMouseButton mouseType = ImGuiMouseButton_Left,
                           int flags = DRAG_FLAG_NONE);
    State   BeginDragState(const char* id, const ImRect& dragRect, const ImVec2& mousePos,
                           ImGuiMouseButton mouseType = ImGuiMouseButton_Left,
                           int flags = DRAG_FLAG_NONE);
    bool    RemoveDragState(UINT id);
    bool    RemoveDragState(const char* id);
    State   GetDragState(UINT id) const;
    State   GetDragState(const char* id) const;
    size_t  GetDraggingCount() const;
    bool    IsDragging(State state) const;
    bool    IsDragging() const;

    const std::unordered_map<ImGuiID, State>& GetDragStateTable() const { return _dragState; }

private:
    std::unordered_map<ImGuiID, State> _dragState;

};
