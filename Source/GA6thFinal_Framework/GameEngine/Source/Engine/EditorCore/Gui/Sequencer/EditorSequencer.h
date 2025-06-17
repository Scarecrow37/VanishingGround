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
    void UpdateCanvas();
    void BeginCanvas();
    void EndCanvas();
    void DrawCanvas();

    int   GetLineUnit() const;
    int   GetLineIndexInt(float x, float unitSize) const;
    float GetLineIndexFloat(float x, float unitSize) const;

public:
    TimelineSystem* _system;

    bool _useSnapping;

    ImRect _canvasRect;
    ImRect _canvasRectUpper;
    ImRect _canvasRectLower;
    float  _upperFrameY;

    float  _viewLerpTarget;
    float  _zoomMin;
    float  _zoomMax;
    ImVec2 _viewPosition;
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    ImU32 UpperBgColor      = IM_COL32(20, 20, 20, 255);
    ImU32 LowerBgColor      = IM_COL32(40, 40, 40, 255);
    ImU32 ThickLineColor    = IM_COL32(120, 120, 120, 200);
    ImU32 ThinLineColor     = IM_COL32(60, 60, 60, 200);
    ImU32 FollowLineColor   = IM_COL32(255, 100, 100, 200);

    float ZoomScale = 1.0f;     // 0 ~ 1 (입실론 값도 고려해야함)
    float UnitSize = 100.0f;
    float ViewLerpScale = 0.02f;
    float ViewScale = 1.0f;
    REFLECT_FIELDS_END(EditorSequencer)
};
