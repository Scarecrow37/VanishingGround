#include "pch.h"
#include "EditorDragState.h"

EditorDragState::EditorDragState() {}

EditorDragState::~EditorDragState() {}

using State = EditorDragState::State;

void EditorDragState::ClearDragState() 
{
    _dragState.clear();
}

void EditorDragState::SetDragState(UINT id, State state)
{
    _dragState[id] = state;
}

void EditorDragState::SetDragState(const char* id, State state) 
{
    if (nullptr != id)
    {
        ImGuiID hash = ImHashStr(id);
        SetDragState(hash, state);
    }
}

State EditorDragState::BeginDragState(UINT id, const ImRect& dragRect, const ImVec2& mousePos,
                                             ImGuiMouseButton mouseType, int flags)
{
    bool isLockFlag  = (flags & DRAG_FLAG_LOCK);
    bool isMovedFlag = (flags & DRAG_FLAG_MOVED);

    if (true == isLockFlag && DRAG_STATE_NONE == GetDragState(id) && true == IsDragging())
    { // 나를 제외한 이미 드래그 상태가 존재하면 리턴
        return DRAG_STATE_NONE;
    }
    ImGuiIO&  io              = ImGui::GetIO();
    State     dragState       = GetDragState(id);
    bool      isRectHovered   = dragRect.Contains(mousePos);
    bool      isMouseDown     = ImGui::IsMouseDown(mouseType);
    bool      isMouseClicked  = ImGui::IsMouseClicked(mouseType);
    bool      isMouseReleased = ImGui::IsMouseReleased(mouseType);
    bool      isMoved         = (io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f);
    switch (dragState)
    {
    case DRAG_STATE_NONE: {
        bool flagsTrigger = (isMovedFlag && isMoved && isMouseDown) || (!isMovedFlag && isMouseClicked);
        if (isRectHovered && flagsTrigger)
        {
            dragState = DRAG_STATE_START;
            SetDragState(id, dragState);
        }
        break;
    }
    case DRAG_STATE_START: {
        dragState = isMouseDown ? DRAG_STATE_DRAGGING : DRAG_STATE_END;
        SetDragState(id, dragState);
        break;
    }
    case DRAG_STATE_DRAGGING: {
        dragState = isMouseDown ? DRAG_STATE_DRAGGING : DRAG_STATE_END;
        SetDragState(id, dragState);
        break;
    }
    case DRAG_STATE_END: {
        RemoveDragState(id);
        break;
    }
    default: {
        dragState = DRAG_STATE_NONE;
        break;
    }
    }
    return dragState;
}

State EditorDragState::BeginDragState(const char* id, const ImRect& dragRect, const ImVec2& mousePos,
                                             ImGuiMouseButton mouseType,
                                             int flags)
{
    ImGuiID hash = ImHashStr(id);
    return BeginDragState(hash, dragRect, mousePos, mouseType, flags);
}

bool EditorDragState::RemoveDragState(UINT id)
{
    auto it = _dragState.find(id);
    if (it != _dragState.end())
    {
        _dragState.erase(it);
        return true;
    }
    return false;
}

bool EditorDragState::RemoveDragState(const char* id)
{
    if (nullptr != id)
    {
        ImGuiID hash = ImHashStr(id);
        RemoveDragState(hash);
    }
    return false;
}

State EditorDragState::GetDragState(UINT id) const
{
    auto it = _dragState.find(id);
    if (it != _dragState.end())
    {
        return it->second;
    }
    return DRAG_STATE_NONE;
}

State EditorDragState::GetDragState(const char* id) const
{
    if (nullptr != id)
    {
        ImGuiID hash = ImHashStr(id);
        return GetDragState(hash);
    }
    return DRAG_STATE_NONE;
}

size_t EditorDragState::GetDraggingCount() const
{
    return _dragState.size();
}

bool EditorDragState::IsDragging(State state) const
{
    return (DRAG_STATE_START == state || DRAG_STATE_DRAGGING == state);
}

bool EditorDragState::IsDragging() const
{
    return 0 != _dragState.size();
}
