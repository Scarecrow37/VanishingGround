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

    /// <summary>
    /// Sequencer의 뷰 위치를 설정합니다.
    /// </summary>
    /// <param name="pos"></param>
    inline void SetViewPosition(const ImVec2& pos) { _viewPos = pos; }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="id"></param>
    inline void SetSelectedNotifyID(UINT id = 0) { _seletedNotifyID = id; }

    void ShowDebugData();

private:
    bool Begin();
    void End();
    void DrawToolBar();
    void DrawCanvas();

    bool WheelZooming();
    bool CanvasDragging();
    bool ContextMenu();

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

    int GetInteractionState(const ImRect& rect) const;

    void DrawNotify(ImDrawList* drawList, TimelineNotify* notify, const ImRect& mainRect);
    void DragNotify(TimelineNotify* notify, const ImRect& mainRect);
    void PopupNotify(TimelineNotify* notify, const ImRect& mainRect);

    float GetCurrentFrame() const;
    float GetMinFrame() const;
    float GetMaxFrame() const;

public:
    std::shared_ptr<TimelineSystem> _system; // System WeakPtr

    UINT    _seletedNotifyID;   // 현재 선택된 Notify의 ID

    bool    _isDebugMode;
    bool    _useSnapping;       // Snap 사용 여부
    bool    _isSnapped;         // 현재 Snap이 적용되었는지 여부

    ImRect _frameRect;          // Sequencer의 전체 프레임 영역
    ImRect _canvasRect;         // Sequencer의 캔버스 전체 영역
    ImRect _canvasRectUpper;    // Sequencer의 캔버스 상단 영역 (타임라인 표시 영역)
    ImRect _canvasRectLower;    // Sequencer의 캔버스 하단 영역 (타임라인 표시 영역)
    float  _canvasUpperHeight;  // Sequencer의 캔버스 상단 영역 높이 (하단 영역은 나머지)

    ImVec2 _viewPos;
    ImVec2 _viewToScaledPos;    // 뷰의 위치를 스케일링한 값 (줌 적용된 위치)
    ImVec2 _zoomPos;

    ImVec2 _mousePos;           // 마우스 커서의 현재 위치 (캔버스 내에서의 위치가 아님)
    ImVec2 _canvasMousePos;     // 마우스 커서가 캔버스 내에서의 위치
    float  _mouseFrame;         // 마우스 커서가 위치한 프레임

    ImVec2 _snapPos;
    ImVec2 _canvasSnapPos;

    ImVec2 _indicatePos;        // 현재 상호작용 등에 사용하는 커서 위치 (스냅, 클램핑 등의 영향을 받아 마우스 커서 위치와 다를 수 있음)
    ImVec2 _canvasIndicatePos;  // 캔버스 내에서의 _indicatePos
    float  _indicateFrame;      // 현재 표시되는 프레임 (클리핑 등으로 인해 마우스 커서가 위치한 프레임과 다를 수 있음)
   
    float   _unitToScaledSize;  // 단위 크기를 스케일링한 값 (줌 적용된 단위 크기)

    float   _viewLerpTarget;    // 보간 중인 최종 뷰의 타겟 위치
    float   _zoomMin;           // 줌 최소 값
    float   _zoomMax;           // 줌 최대 값
     
    EditorDragState _dragHandler;    // 드래그 상태 관리

    std::queue<std::function<void()>> _eventQueue;

    struct InteractionData
    {
        ImVec2 Start;
        ImVec2 End;
    };
    std::vector<InteractionData> _interactionList;

    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    float ZoomScale     = 1.0f;   // View에 대한 줌 스케일
    float UnitSize      = 100.0f; // Frame을 표시할 때 사용하는 단위 크기 (1 Frame당 픽셀 크기)
    float ViewLerpScale = 0.05f;  // View 보간 스케일 (0.0f ~ 1.0f)
    float ViewScale     = 1.0f;   // 현재 View의 스케일

    std::string SerializedData        = "";                           // 직렬화된 데이터

    /* Color (0 = default, 1 = hovered, 2 = pressed 3 = seleted) */
    // Sequencer 상단 배경색
    std::array<ImU32, 3> UpperBgColor = {IM_COL32(20, 20, 20, 255), 0, 0};    
     // Sequencer 하단 배경색 (유효하지 않은 경우)
    std::array<ImU32, 3> LowerInvaildBgColor = {IM_COL32(30, 30, 30, 255), 0, 0};   
    // Sequencer 하단 배경색 (유효한 경우)                                            
    std::array<ImU32, 3> LowerVaildBgColor = {IM_COL32(50, 50, 50, 255), 0, 0};    
    // 두꺼운 선 색상                                                                
    std::array<ImU32, 3> ThickLineColor = {IM_COL32(120, 120, 120, 200), 0, 0}; 
     // 얇은 선 색상                                                                 
    std::array<ImU32, 3> ThinLineColor = {IM_COL32(80, 80, 80, 200), 0, 0};   
    // 최소/최대 프레임 선 색상                                                       
    std::array<ImU32, 3> MinMaxLineColor = {IM_COL32(100, 100, 225, 200), IM_COL32(60, 60, 200, 200), IM_COL32(100, 100, 225, 255) }; 
     // 현재 프레임 선 색상                                                          
    std::array<ImU32, 3> FollowLineColor = {IM_COL32(100, 255, 100, 200), IM_COL32(60, 200, 60, 200), IM_COL32(100, 255, 100, 255) }; 
    // 현재 프레임 선 색상                                                           
    std::array<ImU32, 3> CurFrameLineColor = {IM_COL32(255, 150, 150, 200), IM_COL32(230, 120, 120, 200), IM_COL32(200, 100, 100, 200) }; 
    // Notify 색상                                                                  
    std::array<ImU32, 4> NotifyColor = {IM_COL32(0, 255, 255, 200), IM_COL32(0, 255, 255, 150), IM_COL32(0, 255, 255, 255), IM_COL32(255, 127, 39, 255)};
    // 유효하지 않은 대상에 대한 색상                                                 
    std::array<ImU32, 3> InvalidColor = {IM_COL32(255, 0, 0, 100), 0, 0 };     
    REFLECT_FIELDS_END(EditorSequencer)
};