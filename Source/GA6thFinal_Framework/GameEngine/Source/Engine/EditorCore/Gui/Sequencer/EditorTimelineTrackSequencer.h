#pragma once

namespace Timeline
{
    class EventTrack;
    class EventContext;

    class SequencerEditor : public ReflectSerializer
    {
    public:
        enum Flags : UINT
        {
            FLAGS_NONE                          = 0,

            FLAGS_USE_DEBUG_MODE                = 1 << 0,  // 디버그 모드
            FLAGS_USE_SNAP_MODE                 = 1 << 1,  // Snap 기능 사용 여부

            FLAGS_ALLOW_DRAG_MIN_MAX_LINE       = 1 << 10, // Min/Max 프레임 라인 조정을 잠그는 기능
            FLAGS_ALLOW_DRAG_CURSOR_LINE        = 1 << 11, // 현재 커서 라인 조정을 잠그는 기능
            FLAGS_ALLOW_DRAG_CURRENT_LINE       = 1 << 12, // 현재 프레임 라인 조정을 잠그는 기능
            FLAGS_ALLOW_DRAG_CONTEXT            = 1 << 13, // 이벤트 콘텍스트 조정을 잠그는 기능

            FLAGS_ALLOW_POPUP_CONTEXT_MENU      = 1 << 14, // Context 메뉴 허용 여부
            FLAGS_ALLOW_POPUP_LOWER_CANVAS_MENU = 1 << 15, // Context 메뉴 허용 여부
            FLAGS_ALLOW_POPUP_UPPER_CANVAS_MENU = 1 << 16, // Context 메뉴 허용 여부

            FLAGS_HIDE_MIN_MAX_LINE             = 1 << 20, // Min/Max 프레임 라인 숨김 여부
            FLAGS_HIDE_CURSOR_LINE              = 1 << 21, // 커서 프레임 라인 숨김 여부
            FLAGS_HIDE_CURRENT_LINE             = 1 << 22, // 현재 프레임 라인 숨김 여부
            FLAGS_HIDE_CONTEXT                  = 1 << 23, // 컨텍스트 숨김 여부
            FLAGS_HIDE_CONTEXT_LINE             = 1 << 24, // 컨텍스트 라인 숨김 여부
            FLAGS_HIDE_CONTEXT_LABEL            = 1 << 25, // 컨텍스트 레이블 숨김 여부

            FLAGS_DRAW_CONTEXT_LINE_VERTICAL    = 1 << 30, // 컨텍스트 라인 수직 그리기 여부(이 플래그가 없으면 수평으로 그려짐)

            // 모든 입력 허용
            FLAGS_ALLOW_ALL_INPUT = FLAGS_ALLOW_POPUP_CONTEXT_MENU | 
                                    FLAGS_ALLOW_DRAG_MIN_MAX_LINE | 
                                    FLAGS_ALLOW_DRAG_CURSOR_LINE | 
                                    FLAGS_ALLOW_DRAG_CURRENT_LINE |
                                    FLAGS_ALLOW_DRAG_CONTEXT |      
                                    FLAGS_ALLOW_POPUP_LOWER_CANVAS_MENU |      
                                    FLAGS_ALLOW_POPUP_UPPER_CANVAS_MENU,      
        };

        // 정렬
        enum Align
        {
            ALIGN_LEFT   = 0,
            ALIGN_CENTER = 1,
            ALIGN_RIGHT  = 2,
        };

        // 상호작용 위치 정보 (Snap에 사용)
        struct InteractionData
        {
            ImVec2 Start;
            ImVec2 End;
        };

        // 콜백 함수
        struct Callback
        {
            std::function<void(Timeline::EventTrack&)> LowerFramePopup;
            std::function<void(Timeline::EventTrack&)> UpperFramePopup;
            std::function<void(Timeline::EventTrack&, EventContext&)> ContextPopup;
            std::function<void(Timeline::EventTrack&, EventContext&)> ContextDoubleClick;
        };

    public:
        SequencerEditor();
        virtual ~SequencerEditor();

    public:
        /// <summary>
        /// <para>시퀀서용 Gui를 렌더링합니다.</para>
        /// <para>따로 Frame을 열지 않고 렌더링합니다.</para>
        /// </summary>
        void Show();

        /// <summary>시퀀서의 디버그 정보를 출력합니다.</summary>
        void ShowDebugData();

        /// <summary>시퀀서에서 사용할 TimelineSystem을 설정합니다.</summary>
        /// <param name="system">해당 System에 대한 weak_ptr입니다.</param>
        void SetEventTrack(std::weak_ptr<EventTrack> system);

        /// <summary>시퀀서의 상태를 초기화합니다.</summary>
        void ClearState();

    public:
        /// <summary>시퀀서 에디터가 차지할 크기를 설정합니다. 각 원소가 0일 시 자동으로 크기를 조정합니다.</summary>
        /// <param name="size">설정할 시퀀서의 크기.</param>
        inline void SetSequencerSize(const ImVec2& size) { _sequencerSize = size; }

        /// <summary>마우스 위치의 프레임을 반환합니다.</returns>
        inline float GetFrameFromMousePos() { return _mouseFrame; }

        /// <summary>Sequencer가 가리키는 실제 프레임을 반환합니다.</summary>
        inline float GetFrameFromIndicate() { return _indicateFrame; }

        /// <summary>Sequencer의 Gui영역을 반환합니다.</summary>
        inline ImRect GetFrameRect() const { return _frameRect; }

        /// <summary>Sequencer의 Gui영역 크기를 반환합니다.</summary>
        inline ImVec2 GetRectSize() const { return _frameRect.GetSize(); }

        /// <summary>Sequencer의 Gui영역 위치를 반환합니다.</summary>
        inline ImVec2 GetRectPosition() const { return _frameRect.Min; }

        /// <summary>Sequencer의 뷰 포지션을 반환합니다.</summary>
        inline ImVec2 GetViewPosition() const { return _viewPos; }
        
        /// <summary>Sequencer의 뷰 포지션을 반환합니다.</summary>
        inline float GetViewScale() const { return _viewScale; }

        /// <summary>Sequencer의 콜백을 가져옵니다.</summary>
        inline Callback& GetCallback() { return _callback; }

        /// <summary>
        /// 뷰의 위치를 설정합니다. 
        /// Delay를 사용하면 Lerf를 통해 부드럽게 이동되며, 일반적인 함수는 바로 이동됩니다.
        /// </summary>
        /// <param name="pos"></param>
        inline void   AddViewPosition(const ImVec2& pos) { _viewPos += pos; _targetViewPos += pos; }
        inline void   SetViewPosition(const ImVec2& pos) { _viewPos = pos; _targetViewPos = pos; }
        inline void   AddViewPositionDelay(const ImVec2& pos) { _targetViewPos += pos; }
        inline void   SetViewPositionDelay(const ImVec2& pos) { _targetViewPos = pos; }
        inline void   SetViewPositionDelayFromID(UINT id, Align align = ALIGN_LEFT) { SetViewPositionDelay(GetContextPosition(id) + GetAlginOffsetFromRect(_canvasRectLower, align)); }

        inline void   SetViewScaleDelay(float scale) { _targetViewScale = scale; }
        inline void   SetViewScale(float scale) { _viewScale = scale; _targetViewScale = scale; }
        inline void   AddViewScaleDelay(float scale) { _targetViewScale += scale; }
        inline void   AddViewScale(float scale) { _viewScale += scale; _targetViewScale += scale; }

        inline void   SetSelectedContextID(UINT id = 0) { _seletedContextID = id; }
        inline UINT   GetSelectedContextID() const { return _seletedContextID; }
        
        inline void   SetFlags(UINT flags) { _flags = flags; }
        inline void   AddFlags(UINT flags) { _flags |= flags; }
        inline void   RemoveFlags(UINT flags) { _flags &= ~flags; }
        inline void   ToggleFlags(UINT flags) { _flags ^= flags; }
        inline bool   HasFlags(UINT flags) const { return (_flags & flags) != 0; }

    private:
        bool    Begin();
        void    End();
        void    DrawCanvas();

        void    RefreshTransform();

        bool    WheelZooming();
        bool    CanvasDragging();
        bool    FramePopupMenu();

        void    AddContext(float time, std::string_view label, std::string_view typeNameID);
        void    RemoveContext(EventContext* notify);
        void    ChangeContext(EventContext* notify, float time, std::string_view label, std::string_view typeNameID);
        void    ChangeMinFrame(float frame);
        void    ChangeMaxFrame(float frame);

        ImVec2  PositionToCanvasSapce(const ImVec2& pos) const;
        ImVec2  GetContextPosition(UINT id) const;

        int     GetLineUnit() const;
        float   GetAlignFactor(Align align) const;
        ImVec2  GetAlginOffsetFromRect(const ImRect& rect, Align align) const;
        int     GetFrameFromXToInt(float x, float unitSize) const;
        float   GetFrameFromXToFloat(float x, float unitSize) const;
        int     GetInteractionState(const ImRect& rect) const;
        ImVec2  GetDeltaPosition() const;
        float   GetDeltaScale() const;
        float   GetCurrentFrame() const;
        float   GetMinFrame() const;
        float   GetMaxFrame() const;

    public:
        std::weak_ptr<EventTrack> _track;   // 대상 track의 WeakPtr

        UINT _flags;                        // Sequencer의 플래그 (Flags)
        UINT _seletedContextID;             // 현재 선택된 Context의 ID
        bool _isSnapped;                    // 현재 Snap이 적용되었는지 여부

        Callback _callback;                 // 팝업 콜백 함수
        EditorDragState _dragHandler;       // 드래그 상태 관리

        std::queue<std::function<void()>>   _eventQueue;
        std::vector<InteractionData>        _interactionList;

        /////////////////////////////////////////////////////////////////////////////
        /// Sequencer Transform Data
        /////////////////////////////////////////////////////////////////////////////

        ImVec2 _sequencerSize;          // Sequencer의 전체 크기 (캔버스 크기)

        ImRect _frameRect;              // Sequencer의 전체 프레임 영역
        ImRect _canvasRect;             // Sequencer의 캔버스 전체 영역
        ImRect _canvasRectUpper;        // Sequencer의 캔버스 상단 영역 (타임라인 표시 영역)
        ImRect _canvasRectLower;        // Sequencer의 캔버스 하단 영역 (타임라인 표시 영역)
        ImRect _canvasValidRectLower;   // Sequencer의 캔버스 유효 영역 (하단 영역이 유효한 경우)
        float  _canvasUpperHeight;      // Sequencer의 캔버스 상단 영역 높이 (하단 영역은 나머지)

        ImVec2 _viewPos;                // 현재 뷰의 위치 (캔버스 내에서의 위치가 아님)
        ImVec2 _viewPosPrev;            // 이전 뷰의 위치 (캔버스 내에서의 위치가 아님)
        ImVec2 _targetViewPos;          // 보간 목표 위치
        ImVec2 _viewToScaledPos;        // 뷰의 위치를 스케일링한 값 (줌 적용된 위치)

        float _viewScale;               // 현재 뷰의 스케일 (줌 레벨)
        float _viewScalePrev;           // 이전 뷰의 스케일 (줌 레벨)
        float _targetViewScale;         // 보간 목표 스케일

        ImVec2 _mousePos;               // 마우스 커서의 현재 위치 (캔버스 내에서의 위치가 아님)
        ImVec2 _canvasMousePos;         // 마우스 커서가 캔버스 내에서의 위치
        float  _mouseFrame;             // 마우스 커서가 위치한 프레임

        ImVec2 _snapPos;                // 스냅이 적용된 커서 위치 (캔버스 내에서의 위치가 아님)
        ImVec2 _canvasSnapPos;          // 스냅이 적용된 커서 위치 (캔버스 내에서의 위치)

        ImVec2 _indicatePos;            // 현재 상호작용 등에 사용하는 커서 위치 (스냅, 클램핑 등의 영향을 받아 마우스 커서 위치와 다를 수 있음)
        ImVec2 _canvasIndicatePos;      // 캔버스 내에서의 _indicatePos
        float  _indicateFrame;          // 현재 표시되는 프레임 (클램핑 등으로 인해 마우스 커서가 위치한 프레임과 다를 수 있음)

        float _unitToScaledSize;        // 단위 크기를 스케일링한 값 (줌 적용된 단위 크기)

        ImVec2 _lastContextPosition;    // 마지막으로 Context가 위치한 곳
        ImVec2 _lastContextSize;        // 마지막으로 Context가 위치한 곳의 크기

        ImVec2 _zoomMousePos;           // 줌을 적용할 때의 마우스 위치
        float  _zoomMin;                // 줌 최소 값
        float  _zoomMax;                // 줌 최대 값

        REFLECT_FIELDS_BEGIN(ReflectSerializer)
        float ZoomScale  = 1.0f;   // View에 대한 줌 스케일
        float UnitSize   = 100.0f; // Frame을 표시할 때 사용하는 단위 크기 (1 Frame당 픽셀 크기)
        float LerpFactor = 0.1f;   // View 보간 스케일 (0.0f ~ 1.0f)

        std::string SerializedData = ""; // 직렬화된 데이터

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
        std::array<ImU32, 3> MinMaxLineColor = {IM_COL32(100, 100, 225, 200), IM_COL32(60, 60, 200, 200),
                                                IM_COL32(100, 100, 225, 255)};
        // 현재 프레임 선 색상
        std::array<ImU32, 3> FollowLineColor = {IM_COL32(100, 255, 100, 200), IM_COL32(60, 200, 60, 200),
                                                IM_COL32(100, 255, 100, 255)};
        // 현재 프레임 선 색상
        std::array<ImU32, 3> CurFrameLineColor = {IM_COL32(255, 150, 150, 200), IM_COL32(230, 120, 120, 200),
                                                  IM_COL32(200, 100, 100, 200)};
        // Context 색상
        std::array<ImU32, 4> ContextColor = {IM_COL32(0, 200, 200, 200), IM_COL32(0, 200, 200, 100),
                                             IM_COL32(0, 200, 200, 200), IM_COL32(255, 127, 39, 255)};
        // 유효하지 않은 대상에 대한 색상
        std::array<ImU32, 3> InvalidColor = {IM_COL32(200, 50, 50, 100), 0, 0};
        REFLECT_FIELDS_END(SequencerEditor)

        class Helper
        {
        public:
            static void ProcessUnitLines(SequencerEditor* editor, ImDrawList* drawList);
            static void ProcessMinMaxLine(SequencerEditor* editor, ImDrawList* drawList, std::shared_ptr<EventTrack> track, const char* id, bool isMinLine);
            static void ProcessCurrentFrameLine(SequencerEditor* editor, ImDrawList* drawList, std::shared_ptr<EventTrack> track);
            static void ProcessContexts(SequencerEditor* editor, ImDrawList* drawList, std::shared_ptr<EventTrack> track);
            static void ProcessInteraction(SequencerEditor* editor);
            static void ProcessFollowLine(SequencerEditor* editor, ImDrawList* drawList);

        public:
            /// <summary>캔버스 상단 영역을 그립니다.</summary>
            static void DrawCanvasUpperRect(SequencerEditor* editor, ImDrawList* drawList);

            /// <summary>캔버스 하단 영역을 그립니다.</summary>
            static void DrawCanvasLowerRect(SequencerEditor* editor, ImDrawList* drawList);

            /// <summary>단위 형식의 라인을 그립니다.</summary>
            static void DrawUnitLines(SequencerEditor* editor, ImDrawList* drawList, int frame, const ImVec2& start, const ImVec2& middle, const ImVec2& end);

            /// <summary>Min-Max 형식의 라인을 그립니다.</summary>
            static void DrawMinMaxLines(SequencerEditor* editor, ImDrawList* drawList, const ImVec2& start, const ImVec2& end, int interactState, ImGuiDir direction, float directionHeight = 0.7f);

            /// <summary>현재 프레임의 라인을 그립니다.</summary>
            static void DrawCurrentFrameLine(SequencerEditor* editor, ImDrawList* drawList, const ImVec2& start, const ImVec2& end, const ImRect& rect, int interactState);

            /// <summary>Context의 영역 및 구분 선을 그립니다.</summary>
            static void DrawContext(SequencerEditor* editor, ImDrawList* drawList, EventContext* context, int groupIndex, const ImRect& rect, const ImVec2& offset);

            /// <summary>커서 프레임 라인을 그립니다.</summary>
            static void DrawFollowLine(SequencerEditor* editor, ImDrawList* drawList);

        public:
            static void PathLines(ImDrawList* drawList, ImVec2* points, size_t pointCount);
        };
    };
}
