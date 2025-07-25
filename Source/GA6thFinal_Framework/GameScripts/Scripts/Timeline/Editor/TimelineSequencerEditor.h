#pragma once
namespace Timeline
{
    enum Flags
    {
        SEQUENCER_FLAG_NONE                   = 0,
        SEQUENCER_FLAG_DEBUG_MODE             = 1 << 0, // 디버그 모드
        SEQUENCER_FLAG_USE_SNAP               = 1 << 1, // Snap 기능 사용 여부
        SEQUENCER_FLAG_USE_DRAG_MIN_MAX_FRAME = 1 << 2, // Min/Max 프레임 조정을 잠그는 기능
        SEQUENCER_FLAG_USE_DRAG_FRAME_LINE    = 1 << 3, // 프레임 라인 조정을 잠그는 기능
    };

    enum Align
    {
        ALIGN_LEFT   = 0,
        ALIGN_CENTER = 1,
        ALIGN_RIGHT  = 2,
    };

    class EventTrack;

    template <typename EVENT>
    class SequencerEditor : public ReflectSerializer
    {
    public:
        SequencerEditor();
        virtual ~SequencerEditor();

    public:
        void Show();
        void Clear();
        void SetTrack(std::weak_ptr<EventTrack<EVENT>> track) { _track = track; }

        inline void SetSelectedNotifyID(UINT id = 0) { _seletedNotifyID = id; }
        inline UINT GetSelectedNotifyID() const { return _seletedNotifyID; }

        inline ImRect GetFrameRect() const { return _frameRect; }
        inline ImVec2 GetRectSize() const { return _frameRect.GetSize(); }
        inline ImVec2 GetRectPosition() const { return _frameRect.Min; }

    private:
        std::weak_ptr<EventTrack<EVENT>>  _track;
        std::queue<std::function<void()>> _eventQueue;
        EditorDragState                   _dragHandler;

        UINT    _flags;                 // Sequencer의 플래그 (Flags)
        UINT    _seletedNotifyID;       // 현재 선택된 Notify의 ID

        bool    _isSnapped;             // 현재 Snap이 적용되었는지 여부

        ImRect  _frameRect;             // Sequencer의 전체 프레임 영역
        ImRect  _canvasRect;            // Sequencer의 캔버스 전체 영역
        ImRect  _canvasRectUpper;       // Sequencer의 캔버스 상단 영역 (타임라인 표시 영역)
        ImRect  _canvasRectLower;       // Sequencer의 캔버스 하단 영역 (타임라인 표시 영역)
        float   _canvasUpperHeight;     // Sequencer의 캔버스 상단 영역 높이 (하단 영역은 나머지)

        ImVec2  _viewPos;               // 현재 뷰의 위치 (캔버스 내에서의 위치가 아님)
        ImVec2  _viewPosPrev;           // 이전 뷰의 위치 (캔버스 내에서의 위치가 아님)
        ImVec2  _targetViewPos;         // 보간 목표 위치
        ImVec2  _viewToScaledPos;       // 뷰의 위치를 스케일링한 값 (줌 적용된 위치)

        float   _viewScale;             // 현재 뷰의 스케일 (줌 레벨)
        float   _viewScalePrev;         // 이전 뷰의 스케일 (줌 레벨)
        float   _targetViewScale;       // 보간 목표 스케일

        ImVec2  _mousePos;              // 마우스 커서의 현재 위치 (캔버스 내에서의 위치가 아님)
        ImVec2  _canvasMousePos;        // 마우스 커서가 캔버스 내에서의 위치
        float   _mouseFrame;            // 마우스 커서가 위치한 프레임

        ImVec2  _snapPos;               // 스냅된 마우스 커서 위치 (캔버스 내에서의 위치가 아님)
        ImVec2 _canvasSnapPos;          // 스냅된 마우스 커서가 캔버스 내에서의 위치

        ImVec2  _indicatePos;           // 현재 상호작용 등에 사용하는 커서 위치 (스냅, 클램핑 등의 영향을 받아 마우스 커서 위치와 다를 수 있음)
        ImVec2  _canvasIndicatePos;     // 캔버스 내에서의 _indicatePos
        float   _indicateFrame;         // 현재 표시되는 프레임 (클리핑 등으로 인해 마우스 커서가 위치한 프레임과 다를 수 있음)

        float   _unitToScaledSize;      // 단위 크기를 스케일링한 값 (줌 적용된 단위 크기)

        ImVec2  _lastNotifyPosition;    // 마지막으로 Notify가 위치한 곳
        ImVec2  _lastNotifySize;        // 마지막으로 Notify가 위치한 곳의 크기

        ImVec2  _zoomMousePos;          // 줌을 적용할 때의 마우스 위치
        float   _zoomMin;               // 줌 최소 값
        float   _zoomMax;               // 줌 최대 값

        struct InteractionData
        {
            ImVec2 Start;
            ImVec2 End;
        };
        std::vector<InteractionData> _interactionList;

        REFLECT_FIELDS_BEGIN(ReflectSerializer)
        float ZoomScale             = 1.0f;     // View에 대한 줌 스케일
        float UnitSize              = 100.0f;   // Frame을 표시할 때 사용하는 단위 크기 (1 Frame당 픽셀 크기)
        float LerpFactor            = 0.1f;     // View 보간 스케일 (0.0f ~ 1.0f)
        std::string SerializedData  = "";       // 직렬화된 데이터

        /* Color (0 = default, 1 = hovered, 2 = pressed 3 = seleted) */
        // Sequencer 상단 배경색
        std::array<ImU32, 3> UpperBgColor           = {IM_COL32(20, 20, 20, 255), 0, 0};    
         // Sequencer 하단 배경색 (유효하지 않은 경우)
        std::array<ImU32, 3> LowerInvaildBgColor    = {IM_COL32(30, 30, 30, 255), 0, 0};   
        // Sequencer 하단 배경색 (유효한 경우)                                            
        std::array<ImU32, 3> LowerVaildBgColor      = {IM_COL32(50, 50, 50, 255), 0, 0};    
        // 두꺼운 선 색상                                                                
        std::array<ImU32, 3> ThickLineColor         = {IM_COL32(120, 120, 120, 200), 0, 0}; 
         // 얇은 선 색상                                                                 
        std::array<ImU32, 3> ThinLineColor          = {IM_COL32(80, 80, 80, 200), 0, 0};   
        // 최소/최대 프레임 선 색상                                                       
        std::array<ImU32, 3> MinMaxLineColor        = {IM_COL32(100, 100, 225, 200), IM_COL32(60, 60, 200, 200), IM_COL32(100, 100, 225, 255) }; 
         // 현재 프레임 선 색상                                                          
        std::array<ImU32, 3> FollowLineColor        = {IM_COL32(100, 255, 100, 200), IM_COL32(60, 200, 60, 200), IM_COL32(100, 255, 100, 255) }; 
        // 현재 프레임 선 색상                                                           
        std::array<ImU32, 3> CurFrameLineColor      = {IM_COL32(255, 150, 150, 200), IM_COL32(230, 120, 120, 200), IM_COL32(200, 100, 100, 200) }; 
        // Notify 색상                                                                  
        std::array<ImU32, 4> NotifyColor            = {IM_COL32(0, 255, 255, 200), IM_COL32(0, 255, 255, 150), IM_COL32(0, 255, 255, 255), IM_COL32(255, 127, 39, 255)};
        // 유효하지 않은 대상에 대한 색상                                                 
        std::array<ImU32, 3> InvalidColor           = {IM_COL32(255, 0, 0, 100), 0, 0 };     
        REFLECT_FIELDS_END(EditorSequencer<EVENT>)
    };

    template <typename EVENT>
    inline void SequencerEditor<EVENT>::Show()
    {

    }
    template <typename EVENT>
    inline void SequencerEditor<EVENT>::Clear()
    {
        SetSelectedNotifyID(0);
        _dragHandler.ClearDragState();
    }
}

