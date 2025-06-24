#pragma once
#include "Command/SequencerCommand.h"

class TimelineSystem;

/// <summary>
/// Timeline을 편집할 수 있는 Sequencer입니다.
/// Sequencer는 TimelineSystem을 기반으로 하여 Timeline의 프레임을 편집하고, Notify를 추가하거나 제거할 수 있는 기능을 제공합니다.
/// TimelineSystem은 shared_ptr을 기반으로 생성한 객체만 설정할 수 있습니다.
/// </summary>
class EditorSequencer : public ReflectSerializer
{
public:
    EditorSequencer();
    virtual ~EditorSequencer();

public:
    /// <summary>
    /// <para>Sequencer용 Gui를 렌더링합니다.</para>
    /// <para>따로 Frame을 열지 않고 렌더링합니다.</para>
    /// </summary>
    /// <param name="debug">디버깅 정보를 출력할지 여부</param>
    void Show(bool debug = false);

    /// <summary>
    /// Sequencer에서 사용할 TimelineSystem을 설정합니다.
    /// </summary>
    /// <param name="system">해당 System에 대한 shared_ptr입니다.</param>
    inline void SetSystem(std::shared_ptr<TimelineSystem> system) { _system = system; }

    /// <summary>
    /// Sequencer의 Gui영역을 반환합니다.
    /// </summary>
    /// <returns>Gui영역의 ImRect</returns>
    inline ImRect GetFrameRect() const { return _frameRect; }

    /// <summary>
    /// Sequencer의 Gui영역 크기를 반환합니다.
    /// </summary>
    /// <returns>Gui영역의 크기 ImVec2</returns>
    inline ImVec2 GetRectSize() const { return _frameRect.GetSize(); }

    /// <summary>
    /// Sequencer의 Gui영역 위치를 반환합니다.
    /// </summary>
    /// <returns>Gui영역의 위치 ImVec2</returns>
    ImVec2 GetRectPosition() const { return _frameRect.Min; }

private:
    bool Begin();
    void End();
    void DrawToolBar();
    void DrawCanvas(bool debug);

    bool WheelZooming();
    bool CanvasDragging();
    bool ContextMenu();
    bool IsWheelZooming() const;

    void AddNotify(float time, std::string_view label, std::string_view typeNameID);
    void RemoveNotify(TimelineNotify* notify);
    void ChangeNotify(TimelineNotify* notify, float time, std::string_view label, std::string_view typeNameID);
    void ChangeMinFrame(float frame);
    void ChangeMaxFrame(float frame);

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
    enum DragFlags
    {
        DRAG_FLAG_NONE      = 0,
        DRAG_FLAG_LOCK      = 1 << 0, // 다른 드래그 상태가 존재하면 드래그 상태를 트리거하지 않습니다.
        DRAG_FLAG_MOVED     = 1 << 1  // 마우스가 움직일 때만 드래그 상태가 트리거 됩니다.
    };
    void      SetDragState(const char* id, DragState state);
    DragState BeginDragState(const char* id, const ImRect& dragRect, ImGuiMouseButton mouseType, int flags = DRAG_FLAG_NONE);
    bool      RemoveDragState(const char* id);
    DragState GetDragState(const char* id) const;
    size_t    GetDraggingCount() const;
    bool      CanDrag(const ImRect& dragRect) const;
    bool      IsDragging(DragState state) const;
    bool      IsDragging() const;
    
public:
    std::shared_ptr<TimelineSystem> _system; // System WeakPtr

    bool    _useSnapping;           // Snap 사용 여부

    float   _cursorFrame;           // 마우스 커서가 위치한 프레임
    float   _indicateFrame;         // 현재 표시되는 프레임 (클리핑 등으로 인해 마우스 커서가 위치한 프레임과 다를 수 있음)

    ImRect  _frameRect;             // Sequencer의 전체 프레임 영역
    ImRect  _canvasRect;            // Sequencer의 캔버스 전체 영역
    ImRect  _canvasRectUpper;       // Sequencer의 캔버스 상단 영역 (타임라인 표시 영역)
    ImRect  _canvasRectLower;       // Sequencer의 캔버스 하단 영역 (타임라인 표시 영역)
    float   _canvasUpperHeight;     // Sequencer의 캔버스 상단 영역 높이 (하단 영역은 나머지)

    float   _viewLerpTarget;        // 보간 중인 최종 뷰의 타겟 위치
    float   _zoomMin;               // 줌 최소 값
    float   _zoomMax;               // 줌 최대 값
    ImVec2  _cursorPosition;        // 마우스 커서 위치 (스냅도 적용)
    ImVec2  _viewPosition;          
    ImVec2  _zoomPosition;          

    std::unordered_map<ImGuiID, DragState> _dragState;

    std::queue<std::function<void()>> _eventQueue;

    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    ImU32 UpperBgColor          = IM_COL32(20, 20, 20, 255);    // Sequencer 상단 배경색
    ImU32 LowerInvaildBgColor   = IM_COL32(30, 30, 30, 255);    // Sequencer 하단 배경색 (유효하지 않은 경우)
    ImU32 LowerVaildBgColor     = IM_COL32(50, 50, 50, 255);    // Sequencer 하단 배경색 (유효한 경우)
    ImU32 ThickLineColor        = IM_COL32(120, 120, 120, 200); // 두꺼운 선 색상
    ImU32 ThinLineColor         = IM_COL32(80, 80, 80, 200);    // 얇은 선 색상
    ImU32 MinMaxLineColor       = IM_COL32(100, 100, 225, 200); // 최소/최대 프레임 선 색상
    ImU32 FollowLineColor       = IM_COL32(100, 255, 100, 200); // 현재 프레임 선 색상
    ImU32 CurFrameLineColor     = IM_COL32(255, 150, 150, 200); // 현재 프레임 선 색상
    ImU32 NotifyColor           = IM_COL32(0, 255, 255, 200);   // Notify 색상
    ImU32 InvalidColor          = IM_COL32(255, 0, 0, 100);     // 유효하지 않은 대상에 대한 색상

    float ZoomScale     = 1.0f;         // View에 대한 줌 스케일
    float UnitSize      = 100.0f;       // Frame을 표시할 때 사용하는 단위 크기 (1 Frame당 픽셀 크기)
    float ViewLerpScale = 0.05f;        // View 보간 스케일 (0.0f ~ 1.0f)
    float ViewScale     = 1.0f;         // 현재 View의 스케일

    std::string SerializedData = ""; // 직렬화된 데이터
    REFLECT_FIELDS_END(EditorSequencer)
};