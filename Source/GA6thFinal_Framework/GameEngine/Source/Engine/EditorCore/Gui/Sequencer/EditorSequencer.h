#pragma once

class TimelineSystem;

class EditorSequencer : public ReflectSerializer
{
public:
    EditorSequencer();
    virtual ~EditorSequencer();

public:
    void Render();

    inline void SetSystem(TimelineSystem* system) { _system = system; }

private:
    bool Begin();
    void End();
    void DrawToolBar();
    void DrawCanvas();

    bool WheelZooming();
    bool Dragging();
    bool ContextMenu();
    bool IsWheelZooming() const;
    bool IsDragging(int button) const;

    int   GetLineUnit() const;
    int   GetFrameToInt(float x, float unitSize) const;
    float GetFrameToFloat(float x, float unitSize) const;

    void  SetViewPositionXFromFrame(float frame);

    ImVec2 PositionToCanvasSapce(const ImVec2& pos) const;

public:
    TimelineSystem* _system;

    bool _useSnapping;
    bool _isDraggingCanvas;
    bool _isDraggingStampBar;
    bool _isOpenedPopup;

    float _indicateFrame;

    ImRect _frameRect;
    ImRect _canvasRect;
    ImRect _canvasRectUpper;
    ImRect _canvasRectLower;
    float  _canvasUpperHeight;

    float  _viewLerpTarget;
    float  _zoomMin;
    float  _zoomMax;
    ImVec2 _viewPosition;
    ImVec2 _ZoomPosition;

    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    ImU32 UpperBgColor          = IM_COL32(20, 20, 20, 255);
    ImU32 LowerInvaildBgColor   = IM_COL32(30, 30, 30, 255);
    ImU32 LowerVaildBgColor     = IM_COL32(50, 50, 50, 255);
    ImU32 ThickLineColor        = IM_COL32(120, 120, 120, 200);
    ImU32 ThinLineColor         = IM_COL32(80, 80, 80, 200);
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
