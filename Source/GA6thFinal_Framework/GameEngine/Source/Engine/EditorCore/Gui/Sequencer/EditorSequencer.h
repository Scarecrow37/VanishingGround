#pragma once
#include "Command/SequencerCommand.h"

class TimelineSystem;

class EditorSequencer : public ReflectSerializer
{
public:
    EditorSequencer();
    virtual ~EditorSequencer();

public:
    void Render();

    inline void SetSystem(std::shared_ptr<TimelineSystem> system) { _system = system; }

    ImVec2 GetFrameSize() const;
    ImVec2 GetFramePosition() const;

private:
    bool Begin();
    void End();
    void DrawToolBar();
    void DrawCanvas();

    bool WheelZooming();
    bool CanvasDragging();
    bool ContextMenu();
    bool IsWheelZooming() const;

    int   GetLineUnit() const;
    int   GetFrameFromXToInt(float x, float unitSize) const;
    float GetFrameFromXToFloat(float x, float unitSize) const;

    ImVec2 PositionToCanvasSapce(const ImVec2& pos) const;

    void PathLines(ImDrawList* drawList, ImVec2* points, size_t pointCount) const;

    enum DragState
    {
        DRAG_STATE_NONE = 0,
        DRAG_STATE_START,
        DRAG_STATE_DRAGGING,
        DRAG_STATE_END
    };
    void      SetDragState(const char* id, DragState state);
    DragState BeginDragState(const char* id, const ImRect& dragRect, ImGuiMouseButton mouseType);
    DragState GetDragState(const char* id) const;
    size_t    GetDraggingCount() const;
    bool      IsDragging(DragState state) const;
    
public:
    std::shared_ptr<TimelineSystem> _system;

    bool _useSnapping;
    bool _isOpenedPopup;

    float _cursorFrame;
    float _indicateFrame;

    ImRect _frameRect;
    ImRect _canvasRect;
    ImRect _canvasRectUpper;
    ImRect _canvasRectLower;
    float  _canvasUpperHeight;

    float  _viewLerpTarget;
    float  _zoomMin;
    float  _zoomMax;
    ImVec2 _cursorPosition;
    ImVec2 _viewPosition;
    ImVec2 _ZoomPosition;

    std::unordered_map<ImGuiID, DragState> _dragState;

    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    ImU32 UpperBgColor          = IM_COL32(20, 20, 20, 255);
    ImU32 LowerInvaildBgColor   = IM_COL32(30, 30, 30, 255);
    ImU32 LowerVaildBgColor     = IM_COL32(50, 50, 50, 255);
    ImU32 ThickLineColor        = IM_COL32(120, 120, 120, 200);
    ImU32 ThinLineColor         = IM_COL32(80, 80, 80, 200);
    ImU32 MinMaxLineColor       = IM_COL32(100, 100, 225, 200);
    ImU32 FollowLineColor       = IM_COL32(100, 255, 100, 200);
    ImU32 CurFrameLineColor     = IM_COL32(255, 150, 150, 200);
    ImU32 NotifyColor           = IM_COL32(0, 255, 255, 200);
    ImU32 InvalidColor          = IM_COL32(255, 0, 0, 100);

    float ZoomScale = 1.0f;     // 0 ~ 1 (입실론 값도 고려해야함)
    float UnitSize = 100.0f;
    float ViewLerpScale = 0.02f;
    float ViewScale = 1.0f;

    std::string SerializedData = "";
    REFLECT_FIELDS_END(EditorSequencer)
};