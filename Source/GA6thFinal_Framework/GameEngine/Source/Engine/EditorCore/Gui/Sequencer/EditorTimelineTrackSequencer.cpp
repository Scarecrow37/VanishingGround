#include "pch.h"
#include "EditorTimelineTrackSequencer.h"
#include "Engine/TimelineCore/Track/TimelineEventTrack.h"
#include "Engine/TimelineCore/Context/TimelineEventContext.h"

namespace Timeline
{
    REFLECT_FUNCTION(SequencerEditor)

    SequencerEditor::SequencerEditor()
        : _track(), _flags(0), _isSnapped(false), _mouseFrame(0.0f), _indicateFrame(0.0f), _canvasUpperHeight(10.0f),
          _viewPos(ImVec2(0, 0)), _viewPosPrev(_viewPos), _targetViewPos(ImVec2(0, 0)), _viewToScaledPos(ImVec2(0, 0)),
          _viewScale(1.0f), _viewScalePrev(_viewScale), _targetViewScale(1.0f), _zoomMin(0.05f), _zoomMax(10.0f)
    {
    }

    SequencerEditor::~SequencerEditor() = default;

    void SequencerEditor::Show()
    {
        // 트랙이 유효하지 않다면 return
        if (true == _track.expired())
        {
            _track.reset();
            return;
        }

        ImGui::PushID(this);
        Begin();
        DrawCanvas();
        End();
        ImGui::PopID();
    }

    void SequencerEditor::SetEventTrack(std::weak_ptr<EventTrack> system)
    {
        // 이미 같은 시스템이 설정되어 있다면 아무 작업도 하지 않음
        if (system.lock() != _track.lock())
        {
            _track = system;
            ClearState();
        }
    }

    void SequencerEditor::ClearState() 
    {
        SetSelectedContextID(0);
        _dragHandler.ClearDragState();
    }

    void SequencerEditor::ShowDebugData()
    {
        ImGui::Text("Selected Context ID: %d", _seletedContextID);
        if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("View Position: (%.2f, %.2f)", _viewPos.x, _viewPos.y);
            ImGui::Text("View Previous Position: (%.2f, %.2f)", _viewPosPrev.x, _viewPosPrev.y);
            ImGui::Text("Target View Position: (%.2f, %.2f)", _targetViewPos.x, _targetViewPos.y);
            ImGui::Text("View Scale: %.2f", _viewScale);
            ImGui::Text("Previous View Scale: %.2f", _viewScalePrev);
            ImGui::Text("Target View Scale: %.2f", _targetViewScale);
            ImGui::Text("Zoom Position: (%.2f, %.2f)", _zoomMousePos.x, _zoomMousePos.y);
            ImGui::TreePop();
        }
        if (ImGui::TreeNodeEx("Rect", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Frame Rect: (%.2f, %.2f, %.2f, %.2f)", _frameRect.Min.x, _frameRect.Min.y, _frameRect.Max.x,
                        _frameRect.Max.y);
            ImGui::Text("Canvas Rect: (%.2f, %.2f, %.2f, %.2f)", _canvasRect.Min.x, _canvasRect.Min.y,
                        _canvasRect.Max.x, _canvasRect.Max.y);
            ImGui::Text("Canvas Upper Rect: (%.2f, %.2f, %.2f, %.2f)", _canvasRectUpper.Min.x, _canvasRectUpper.Min.y,
                        _canvasRectUpper.Max.x, _canvasRectUpper.Max.y);
            ImGui::Text("Canvas Lower Rect: (%.2f, %.2f, %.2f, %.2f)", _canvasRectLower.Min.x, _canvasRectLower.Min.y,
                        _canvasRectLower.Max.x, _canvasRectLower.Max.y);
            ImGui::TreePop();
        }
        if (ImGui::TreeNodeEx("Mouse", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Mouse Position: (%.2f, %.2f)", _mousePos.x, _mousePos.y);
            ImGui::Text("Canvas Mouse Position: (%.2f, %.2f)", _canvasMousePos.x, _canvasMousePos.y);
            ImGui::Text("Mouse Frame: %.2f", _mouseFrame);
            ImGui::TreePop();
        }
        if (ImGui::TreeNodeEx("Indicate", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Indicate Position: (%.2f, %.2f)", _indicatePos.x, _indicatePos.y);
            ImGui::Text("Canvas Indicate Position: (%.2f, %.2f)", _canvasIndicatePos.x, _canvasIndicatePos.y);
            ImGui::Text("Indicate Frame: %.2f", _indicateFrame);
            ImGui::TreePop();
        }
        if (ImGui::TreeNodeEx("Snap", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Snap Position: (%.2f, %.2f)", _snapPos.x, _snapPos.y);
            ImGui::Text("Canvas Snap Position: (%.2f, %.2f)", _canvasSnapPos.x, _canvasSnapPos.y);
            ImGui::TreePop();
        }

        auto& dragStateTable = _dragHandler.GetDragStateTable();
        if (ImGui::TreeNodeEx("Drag State", ImGuiTreeNodeFlags_DefaultOpen))
        {
            for (auto& state : dragStateTable)
            {
                ImGui::Text("Drag State ID: %d, State: %d", state.first, state.second);
            }
            ImGui::TreePop();
        }
    }

    bool SequencerEditor::Begin()
    {
        ImGuiIO&    io       = ImGui::GetIO();
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        FramePopupMenu();
        WheelZooming();
        CanvasDragging();

        // 위치 및 사이즈 갱신
        RefreshTransform();

        // 그릴 공간에 대한 여백 만들어놓기
        ImGui::Dummy(_canvasRect.GetSize());

        // 캔버스에 대한 클리핑 영역 지정 (라인이 넘어가지 않도록)
        ImGui::PushClipRect(_canvasRect.Min, _canvasRect.Max, true);

        // 캔버스 상단 영역 그리기.
        Helper::DrawCanvasUpperRect(this, drawList);
        // 캔버스 하단 영역 그리기.
        Helper::DrawCanvasLowerRect(this, drawList);

        // Begin 성공 여부 반환
        bool isValid = GetMaxFrame() >= GetMinFrame();
        return isValid;
    }

    void SequencerEditor::End()
    {
        ImGui::PopClipRect();

        while (false == _eventQueue.empty())
        {
            auto event = _eventQueue.front();
            if (event)
            {
                event();
            }
            _eventQueue.pop();
        }
    }

    void SequencerEditor::DrawCanvas()
    {
        auto  track    = _track.lock();
        auto& io       = ImGui::GetIO();
        auto* drawList = ImGui::GetWindowDrawList();
        _interactionList.clear();
        _isSnapped = false;

        drawList->AddRectFilled(_canvasValidRectLower.Min, _canvasValidRectLower.Max, ReflectFields->LowerVaildBgColor[0]);

        // 단위 선 그리기
        Helper::ProcessUnitLines(this, drawList);

        // 최소, 최대 프레임 선 드래그 이벤트 처리 및 그리기
        Helper::ProcessMinMaxLine(this, drawList, track, "MinFrameLine", true);
        Helper::ProcessMinMaxLine(this, drawList, track, "MaxFrameLine", false);

        // 현재 프레임 선 드래그 이벤트 처리 및 그리기
        Helper::ProcessCurrentFrameLine(this, drawList, track);

        // 콘텍스트 드래그 이벤트 처리 및 그리기
        Helper::ProcessContexts(this, drawList, track);

        // 상호 작용 처리
        Helper::ProcessInteraction(this);

        // 따라다니는 선 그리기
        Helper::ProcessFollowLine(this, drawList);
    }

    void SequencerEditor::RefreshTransform()
    {
        ImGuiIO& io         = ImGui::GetIO();
        float minFrame      = GetMinFrame();
        float maxFrame      = GetMaxFrame();

        ImVec2 windowPos    = ImGui::GetCursorScreenPos();
        ImVec2 frameSize    = ImGui::GetContentRegionAvail();
        ImVec2 canvasSize   = _sequencerSize;
        bool autoSizeX      = _sequencerSize.x == 0.0f;
        bool autoSizeY      = _sequencerSize.y == 0.0f;
        canvasSize.x        = autoSizeX ? frameSize.x : _sequencerSize.x;
        canvasSize.y        = autoSizeY ? frameSize.y : _sequencerSize.y;

        /// Frame Rect /////////////////////////////////////////////
        _frameRect = ImRect(windowPos, windowPos + frameSize);
        /// Canvas Rect /////////////////////////////////////////////
        _canvasRect = ImRect(windowPos, windowPos + canvasSize);
        /// Canvas Rect /////////////////////////////////////////////
        _canvasUpperHeight = 20.0f;
        _canvasRectUpper = ImRect(_canvasRect.Min, ImVec2(_canvasRect.Max.x, _canvasRect.Min.y + _canvasUpperHeight));
        _canvasRectLower = ImRect(ImVec2(_canvasRect.Min.x, _canvasRect.Min.y + _canvasUpperHeight), _canvasRect.Max);
        /// View Lerp /////////////////////////////////////////////////////
        float lerpT = ReflectFields->LerpFactor;

        _targetViewScale = ImClamp(_targetViewScale, _zoomMin, _zoomMax);
        _viewPosPrev     = _viewPos;
        _viewScalePrev   = _viewScale;

        _viewPos   = ImLerp(_viewPos, _targetViewPos, lerpT);
        _viewScale = ImLerp(_viewScale, _targetViewScale, lerpT);
        _viewScale = ImClamp(_viewScale, _zoomMin, _zoomMax);

        bool isZoomPosInvalid = std::isnan(_zoomMousePos.x); 
        if (false == isZoomPosInvalid)
        {   // zoom focus calculation
            ImVec2 pre, post;
            pre.x  = _zoomMousePos.x / _viewScalePrev;
            post.x = _zoomMousePos.x / _viewScale;
            AddViewPosition(post - pre);
        }
        /// MousePos /////////////////////////////////////////////////////
        _mousePos       = io.MousePos;
        _canvasMousePos = io.MousePos - _canvasRect.Min;
        /// Snap /////////////////////////////////////////////////////
        _snapPos       = ImVec2(0.0f, 0.0f);
        _canvasSnapPos = ImVec2(0.0f, 0.0f);
        /// Scaled  /////////////////////////////////////////////////////
        _viewToScaledPos  = _viewPos * _viewScale;
        _unitToScaledSize = ReflectFields->UnitSize * _viewScale;
        /// Canvas Valid Rect Lower //////////////////////////////////////
        ImVec2 validRectMin = ImVec2(_viewToScaledPos.x + (minFrame * _unitToScaledSize), 0.0f) + _canvasRectLower.Min;
        ImVec2 validRectMax = ImVec2(_viewToScaledPos.x + (maxFrame * _unitToScaledSize), canvasSize.y) + _canvasRectLower.Min;
        _canvasValidRectLower = ImRect(validRectMin, validRectMax);
    }

    bool SequencerEditor::WheelZooming()
    {
        ImGuiIO& io           = ImGui::GetIO();
        bool     isContain    = _canvasRect.Contains(io.MousePos);
        bool     isMouseValid = ImGui::IsMousePosValid();
        bool     isWheelMoved = (io.MouseWheel < -FLT_EPSILON || io.MouseWheel > FLT_EPSILON);
        bool     isZooming    = isContain && isMouseValid && isWheelMoved;

        if (true == isZooming)
        {
            float targetViewScale = _targetViewScale * ReflectFields->ZoomScale;
            if (io.MouseWheel < -FLT_EPSILON)
            {
                SetViewScaleDelay(targetViewScale * 0.9f);
            }
            if (io.MouseWheel > FLT_EPSILON)
            {
                SetViewScaleDelay(targetViewScale * 1.1f);
            }
            _zoomMousePos = io.MousePos - _frameRect.Min;
        }
        return isZooming;
    }

    bool SequencerEditor::CanvasDragging()
    {
        ImGuiIO&    io        = ImGui::GetIO();
        const char* id        = "CanvasFrame";
        bool        isContain = _canvasRect.Contains(_mousePos);
        if (false == isContain)
        {
            return false;
        }
        int  flags      = EditorDragState::DRAG_FLAG_LOCK | EditorDragState::DRAG_FLAG_MOVED;
        int  state      = _dragHandler.BeginDragState(id, _canvasRect, _indicatePos, ImGuiMouseButton_Right, flags);
        bool isDragging = (state == EditorDragState::DRAG_STATE_DRAGGING);
        if (true == isDragging)
        {
            bool isMoved = (io.MouseDelta.x != 0.0f);
            if (true == isMoved)
            {
                AddViewPositionDelay(ImVec2(io.MouseDelta.x / _viewScale, 0.0f));
            }
        }
        return isDragging;
    }

    bool SequencerEditor::FramePopupMenu()
    {
        ImGuiIO& io             = ImGui::GetIO();
        bool     isMouseRBUp    = ImGui::IsMouseReleased(ImGuiMouseButton_Right);
        bool     isLowerContain = _canvasRectLower.Contains(io.MousePos);
        bool     isUpperContain = _canvasRectUpper.Contains(io.MousePos);
        bool     isNoneDragging = false == _dragHandler.IsDragging();

        const char* lowerPopupID = "popup##lower popup";
        const char* upperPopupID = "popup##upper popup";

        auto track = _track.lock();
        if (isMouseRBUp && isNoneDragging)
        {
            if (isLowerContain)
            {
                if (HasFlags(FLAGS_ALLOW_POPUP_LOWER_CANVAS_MENU))
                {
                    ImGui::OpenPopup(lowerPopupID);
                }
            }
            if (isUpperContain)
            {
                if (HasFlags(FLAGS_ALLOW_POPUP_UPPER_CANVAS_MENU))
                {
                    ImGui::OpenPopup(upperPopupID);
                }
            }
        }

        if (ImGui::BeginPopup(lowerPopupID))
        {
            ImGuiHelper::AlignedText("Canvas Menu", ImGuiHelper::CENTER, 0.8f);
            ImGui::Separator();
            if (ImGui::BeginMenu("Mode"))
            {
                if (ImGui::MenuItem("Debug", "", HasFlags(FLAGS_USE_DEBUG_MODE)))
                {
                    ToggleFlags(FLAGS_USE_DEBUG_MODE);
                }

                if (ImGui::MenuItem("Snap", "", HasFlags(FLAGS_USE_SNAP_MODE)))
                {
                    ToggleFlags(FLAGS_USE_SNAP_MODE);
                }
                ImGui::EndMenu();
            }
            // Custom Callback
            if (track && _callback.LowerFramePopup)
            {
                ImGui::Separator();
                _callback.LowerFramePopup(*track.get());
            }
            ImGui::EndPopup();
        }

        if (ImGui::BeginPopup(upperPopupID))
        {
            ImGuiHelper::AlignedText("Canvas Menu", ImGuiHelper::CENTER, 0.8f);
            ImGui::Separator();
            // Custom Callback
            if (track && _callback.UpperFramePopup)
            {
                ImGui::Separator();
                _callback.UpperFramePopup(*track.get());
            }
            ImGui::EndPopup();
        }
        return true;
    }

    void SequencerEditor::AddContext(float time, std::string_view label, std::string_view typeNameID)
    {
        _eventQueue.push([this, time, label, typeNameID]() {
            //UmCommandManager.Do<Command::Sequencer::AddContext>(_track, time, label, typeNameID);
        });
    }

    void SequencerEditor::RemoveContext(EventContext* context)
    {
        _eventQueue.push([this, context]() {
            _dragHandler.RemoveDragState(context->ID);
            // UmCommandManager.Do<Command::Sequencer::RemoveContext>(_track, context);
            if (false == _track.expired())
            {
                auto track = _track.lock();
                track->RemoveContextFromID(context->ID);
            }
        });
    }

    void SequencerEditor::ChangeContext(EventContext* context, float time, std::string_view label,
                                       std::string_view typeNameID)
    {
        _eventQueue.push([this, context, time, label, typeNameID]() {
            //UmCommandManager.Do<Command::Sequencer::ChangeContext>(_track, context, time, label, typeNameID);
        });
    }

    void SequencerEditor::ChangeMinFrame(float frame)
    {
        _eventQueue.push([this, frame]() {
            //UmCommandManager.Do<Command::Sequencer::ChangeMinFrame>(_track, frame);
            if (false == _track.expired())
            {
                auto track = _track.lock();
                track->SetMinFrame(frame);
            }
        });
    }

    void SequencerEditor::ChangeMaxFrame(float frame)
    {
        _eventQueue.push([this, frame]() {
            //UmCommandManager.Do<Command::Sequencer::ChangeMaxFrame>(_track, frame);
            if (false == _track.expired())
            {
                auto track = _track.lock();
                track->SetMaxFrame(frame);
            }
        });
    }

    int SequencerEditor::GetLineUnit() const
    {
        int unitFactor = 1;
        if (_viewScale < 0.1f)
        {
            unitFactor = 10;
        }
        else if (_viewScale < 0.4f)
        {
            unitFactor = 5;
        }
        else
        {
            unitFactor = 1;
        }
        return unitFactor;
    }

    float SequencerEditor::GetAlignFactor(Align align) const
    {
        // ALIGN_LEFT = 0.0f, ALIGN_CENTER = 0.5f, ALIGN_RIGHT = 1.0f
        if (align == Align::ALIGN_LEFT)
        {
            return 0.0f;
        }
        else if (align == Align::ALIGN_CENTER)
        {
            return 0.5f;
        }
        else if (align == Align::ALIGN_RIGHT)
        {
            return 1.0f;
        }
        return 0.0f;
    }

    ImVec2 SequencerEditor::GetAlginOffsetFromRect(const ImRect& rect, Align align) const
    {
        ImVec2 result;
        float  alignFactorToView = GetAlignFactor(align) / _viewScale;
        result                   = rect.GetSize() * alignFactorToView;
        return result;
    }

    int SequencerEditor::GetFrameFromXToInt(float x, float unitSize) const
    {
        return static_cast<int>(GetFrameFromXToFloat(x, unitSize));
    }

    float SequencerEditor::GetFrameFromXToFloat(float x, float unitSize) const
    {
        return x / unitSize;
    }
    ImVec2 SequencerEditor::PositionToCanvasSapce(const ImVec2& pos) const
    {
        return ImVec2(_viewPos + pos) * _viewScale;
    }

    ImVec2 SequencerEditor::GetContextPosition(UINT id) const
    {
        ImVec2 result;
        if (false == _track.expired())
        {
            auto system = _track.lock();
            auto context = system->GetContextFromID(id);
            if (context)
            {
                float time = context->Time;
                result.x   = -(time * ReflectFields->UnitSize);
                result.y   = 0.0f; // Y position is not used in this context
            }
        }
        return result;
    }

    int SequencerEditor::GetInteractionState(const ImRect& rect) const
    {
        ImGuiIO& io          = ImGui::GetIO();
        bool     isHovered   = rect.Contains(io.MousePos);
        bool     isMouseDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);
        int      state       = 0;
        if (true == isHovered)
        {
            state = isMouseDown ? 2 : 1; // 2: Pressed, 1: Hovered
        }
        return state;
    }

    ImVec2 SequencerEditor::GetDeltaPosition() const
    {
        return _viewPos - _viewPosPrev;
    }

    float SequencerEditor::GetDeltaScale() const
    {
        return _viewScale - _viewScalePrev;
    }

    float SequencerEditor::GetCurrentFrame() const
    {
        if (true == _track.expired())
            return 0.0f;
        auto system = _track.lock();
        return system->GetCurrentFrame();
    }

    float SequencerEditor::GetMinFrame() const
    {
        if (true == _track.expired())
            return 0.0f;
        auto system = _track.lock();
        return system->GetMinFrame();
    }

    float SequencerEditor::GetMaxFrame() const
    {
        if (true == _track.expired())
            return 0.0f;
        auto system = _track.lock();
        return system->GetMaxFrame();
    }

    void SequencerEditor::Helper::ProcessUnitLines(SequencerEditor* editor, ImDrawList* drawList) 
    {
        const int linePerFrame = editor->GetLineUnit();
        const float unitPerPixel = editor->ReflectFields->UnitSize;

        ImVec2& viewPos = editor->_viewPos;
        ImVec2& viewToScaledPos  = editor->_viewToScaledPos;
        float&  unitToScaledSize = editor->_unitToScaledSize;
        ImVec2  canvasSize       = editor->_canvasRect.GetSize();

        ImRect& canvasRectUpper  = editor->_canvasRectUpper;
        ImRect& canvasRectLower  = editor->_canvasRectLower;

        const float startX = fmodf(viewToScaledPos.x, unitToScaledSize);

        int curFrame = editor->GetFrameFromXToInt(-viewPos.x, unitPerPixel);
        for (float x = startX; x < canvasSize.x; x += unitToScaledSize, ++curFrame)
        {
            if (curFrame % linePerFrame != 0)
            {
                continue;
            }
            ImVec2 start  = ImVec2(x, 0.0f) + canvasRectUpper.Min;
            ImVec2 middle = ImVec2(x, 0.0f) + canvasRectLower.Min;
            ImVec2 end    = ImVec2(x, canvasSize.y) + canvasRectLower.Min;

            DrawUnitLines(editor, drawList, curFrame, start, middle, end);

            if (editor->HasFlags(FLAGS_USE_SNAP_MODE))
            {
                editor->_interactionList.emplace_back(start, end);
            } 
        }

    }

    void SequencerEditor::Helper::ProcessMinMaxLine(SequencerEditor* editor, ImDrawList* drawList, std::shared_ptr<EventTrack> track, const char* id, bool isMinLine)
    {
        if (nullptr == editor || nullptr == drawList || nullptr == track)
        {
            return;
        }
        if (true == editor->HasFlags(FLAGS_HIDE_MIN_MAX_LINE))
        {
            return;
        }

        ImVec2& indicatePos = editor->_indicatePos;
        ImRect& rect        = editor->_canvasValidRectLower;
        auto&   dragHandler = editor->_dragHandler;
        ImVec2  start       = isMinLine ? ImVec2(rect.Min.x, rect.Min.y) : ImVec2(rect.Max.x, rect.Min.y);
        ImVec2  end         = ImVec2(start.x, rect.Max.y);
        ImRect  dragRect    = ImRect(start + ImVec2(-2.0f, 0.0f), end + ImVec2(2.0f, 0.0f));

        bool isDraggable = false == dragHandler.IsDragging() || dragHandler.IsDraggingOnly(id);
        bool canDrag     = editor->HasFlags(FLAGS_ALLOW_DRAG_MIN_MAX_LINE) && isDraggable;

        int dragState = EditorDragState::DRAG_STATE_NONE;
        if (true == canDrag)
        {
            dragState = dragHandler.BeginDragState(id, dragRect, indicatePos);
        }
        switch (dragState)
        {
            case EditorDragState::DRAG_STATE_NONE: {
                editor->_interactionList.emplace_back(start, end);
                break;
            }
            case EditorDragState::DRAG_STATE_START: {
                float frame = isMinLine ? track->GetMinFrame() : track->GetMaxFrame();
                isMinLine ? editor->ChangeMinFrame(frame) : editor->ChangeMaxFrame(frame);
                break;
            }
            case EditorDragState::DRAG_STATE_DRAGGING: {
                float mouseFrame = editor->_mouseFrame;
                isMinLine ? track->SetMinFrame(mouseFrame) : track->SetMaxFrame(mouseFrame);
                break;
            }
            default:
                break;
        }

        int      interacted = editor->GetInteractionState(dragRect);
        ImGuiDir direction  = isMinLine ? ImGuiDir_Right : ImGuiDir_Left;
        DrawMinMaxLines(editor, drawList, start, end, interacted, direction);
    }
    void SequencerEditor::Helper::ProcessCurrentFrameLine(SequencerEditor* editor, ImDrawList* drawList,
                                                   std::shared_ptr<EventTrack> track)
    {
        if (nullptr == editor || nullptr == drawList || nullptr == track)
        {
            return;
        }
        if (true == editor->HasFlags(FLAGS_HIDE_CURRENT_LINE))
        {
            return;
        }

        const char* id        = "StampBar";
        auto&   dragHandler   = editor->_dragHandler;
        ImRect& canvas        = editor->_canvasRectLower;
        float   curFrame      = editor->GetCurrentFrame(); // current frame in the timeline
        float   unitSize      = editor->ReflectFields->UnitSize;
        ImVec2  indicatePos   = editor->_indicatePos;
        float   indicateFrame = editor->_indicateFrame;

        ImVec2 linePos     = ImVec2(curFrame * unitSize, 0.0f);
        ImVec2 canvasSapce = ImVec2(editor->PositionToCanvasSapce(linePos).x, 0.0f);

        ImVec2 start = canvasSapce + canvas.Min;
        ImVec2 end   = start + ImVec2(0.0f, canvas.GetSize().y);

        if (EditorDragState::DRAG_STATE_NONE == dragHandler.GetDragState(id))
        { // 스탬프바 드래깅 중이 아닐 때만 상호작용을 한다.
            editor->_interactionList.emplace_back(start, end);
        }
        // Draw Point Rect
        float  tipDepth = 0.3f;
        float  height   = editor->_canvasUpperHeight;
        ImVec2 size(height * 0.5f, height);
        ImVec2 pos(start.x - (size.x * 0.5f), start.y - size.y);
        ImRect rect(pos, pos + (size * ImVec2(1.0f, 1.0f - tipDepth)));
        ImRect pointRect(pos, pos + size);

        bool isHovered      = pointRect.Contains(editor->_mousePos);
        bool isMouseDown    = ImGui::IsMouseDown(ImGuiMouseButton_Left);
        bool isMouseClicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
        int  interacted     = editor->GetInteractionState(rect);

        bool isDraggable    = false == dragHandler.IsDragging() || dragHandler.IsDraggingOnly(id);
        bool canDrag        = editor->HasFlags(FLAGS_ALLOW_DRAG_CURRENT_LINE) && isDraggable;

        if (true == canDrag)
        {
            int  dragState  = dragHandler.BeginDragState(id, pointRect, indicatePos);
            bool isDragging = dragHandler.IsDragging(dragState);
            if (true == isDragging)
            {
                float minFrame = editor->GetMinFrame();
                float maxFrame = editor->GetMaxFrame();
                float frame    = ImClamp(indicateFrame, minFrame, maxFrame);
                track->SetCurrentFrame(frame);
            }
        }
        DrawCurrentFrameLine(editor, drawList, start, end, rect, interacted);
    }
    void SequencerEditor::Helper::ProcessContexts(SequencerEditor* editor, ImDrawList* drawList, std::shared_ptr<EventTrack> track)
    {
        if (nullptr == editor || nullptr == drawList || nullptr == track)
        {
            return;
        }
        if (true == editor->HasFlags(FLAGS_HIDE_CONTEXT))
        {
            return;
        }
        const auto& contextList = track->GetEventContextQueue();
        std::unordered_map<int, size_t> paddingGroup; // groupIndex -> 현재 레이어 수
        for (size_t i = 0; i < contextList.size(); ++i)
        {
            /// Context Information
            auto* context            = contextList[i];
            UINT             id      = context->ID;
            float            time    = context->Time;
            std::string_view label   = context->Label;

            ImVec2& indicatePos = editor->_indicatePos;
            ImVec2& mousePos    = editor->_mousePos;
            auto&   dragHandler = editor->_dragHandler;
            float curFrame      = context->Time;
            float unitSize      = editor->_unitToScaledSize;
            float linePerFrame  = (float)editor->GetLineUnit();
            int   groupIndex    = static_cast<int>(std::floor(curFrame / linePerFrame));
            float layer         = (float)paddingGroup[groupIndex]++; // 현재 그룹에서 사용될 패딩 레이어

            const float  length    = 7.0f;
            const float  paddingY  = 40.0f + layer * 25.0f;

            ImVec2 offset = ImVec2(editor->_viewToScaledPos.x, paddingY) + editor->_canvasRectLower.Min;
            ImVec2 point  = offset + ImVec2(curFrame * unitSize, 0.0f);
            ImRect rect   = ImRect(point - ImVec2(length, length), point + ImVec2(length, length));
            
            bool isSelected = (id == editor->_seletedContextID);

            /// Process Context Dragging
            bool isDraggable = false == dragHandler.IsDragging() || dragHandler.IsDraggingOnly(id);
            bool canDrag     = editor->HasFlags(FLAGS_ALLOW_DRAG_CONTEXT) && isDraggable;
            int state = EditorDragState::DRAG_STATE_NONE;
            if (true == canDrag && true == isSelected)
            {
                state = dragHandler.BeginDragState(id, rect, indicatePos);
            }
            switch (state)
            {
                case EditorDragState::DRAG_STATE_NONE:
                {
                    ImVec2 center = rect.GetCenter();
                    editor->_interactionList.emplace_back(center, center);
                    break;
                }
                case EditorDragState::DRAG_STATE_START:
                    break;
                case EditorDragState::DRAG_STATE_DRAGGING:
                    track->ChangeContextTime(id, editor->_indicateFrame);
                    break;
                default:
                    break;
            }

            /// Process Context PopupMenu
            ImGui::PushID(context);
            const char* contextPopupID = "##context popup";
            bool isHovered = rect.Contains(mousePos);
            if (true == isHovered)
            {
                if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                {
                    if (editor->_callback.ContextDoubleClick)
                    {
                        editor->_callback.ContextDoubleClick(*track.get(), *context);
                    }
                }
                bool rUp = ImGui::IsMouseReleased(ImGuiMouseButton_Right);
                bool lUp = ImGui::IsMouseReleased(ImGuiMouseButton_Left);
                if (true == rUp)
                {
                    if (false == dragHandler.IsDragging())
                    {
                        ImGui::OpenPopup(contextPopupID);
                    }
                }
                if (false == isSelected)
                {
                    if (true == rUp || true == lUp)
                    {
                        editor->SetSelectedContextID(id);
                    }
                }
            }
            if (ImGui::BeginPopup(contextPopupID))
            {
                ImGuiHelper::AlignedText("Context Menu", ImGuiHelper::CENTER, 0.8f);
                ImGui::Separator();
                if (true == editor->HasFlags(FLAGS_USE_DEBUG_MODE))
                {
                    ImGui::Text("Time: %f", time);
                    ImGui::Text("ID: %d", id);
                    ImGui::Separator();
                }
                if (ImGui::MenuItem("Remove Context"))
                {
                    editor->RemoveContext(context);
                }
                // Custom Callback
                if (track && editor->_callback.ContextPopup)
                {
                    ImGui::Separator();
                    editor->_callback.ContextPopup(*track.get(), *context);
                }
                ImGui::EndPopup();
            }
            ImGui::PopID();
            /// Draw Context
            DrawContext(editor, drawList, context, groupIndex, rect, offset);
        }
    }
    void SequencerEditor::Helper::ProcessInteraction(SequencerEditor* editor) 
    {
        if (nullptr == editor)
        {
            return;
        }
        float  minDistance  = FLT_MAX;
        ImVec2 mousePos     = editor->_mousePos;
        ImVec2 bestSnapPos  = mousePos;
        bool   foundSnap    = false;
        float  linePerFrame = (float)editor->GetLineUnit();
        float  snapRange    = (editor->_unitToScaledSize * linePerFrame) * 0.1f;
        bool   isContain    = editor->_canvasRect.Contains(mousePos);
        if (true == editor->HasFlags(FLAGS_USE_SNAP_MODE) && true == isContain)
        {
            for (const auto& line : editor->_interactionList)
            {
                float distance  = fabsf(mousePos.x - line.Start.x);
                if (distance <= snapRange && distance < minDistance)
                {
                    minDistance = distance;
                    bestSnapPos = ImVec2(line.Start.x, mousePos.y);
                    foundSnap   = true; 
                }
            }
        }
        if (true == foundSnap)
        {
            editor->_isSnapped      = true;
            editor->_snapPos        = bestSnapPos;
            editor->_canvasSnapPos  = editor->_snapPos - editor->_canvasRect.Min;
        }
        editor->_indicatePos        = editor->_isSnapped ? editor->_snapPos : editor->_mousePos;
        editor->_canvasIndicatePos  = editor->_isSnapped ? editor->_canvasSnapPos : editor->_canvasMousePos;

        float canvasSpaceX   = -editor->_viewPos.x + editor->_canvasIndicatePos.x / editor->_viewScale;
        bool  anyPopupOpened = ImGui::IsPopupOpen("", ImGuiPopupFlags_AnyPopupId);
        if (false == anyPopupOpened)
        {
            float minFrame         = editor->GetMinFrame();
            float maxFrame         = editor->GetMaxFrame();
            float unitSize         = editor->ReflectFields->UnitSize;
            editor->_mouseFrame    = editor->GetFrameFromXToFloat(canvasSpaceX, unitSize);
            editor->_indicateFrame = ImClamp(editor->_mouseFrame, minFrame, maxFrame);
        }
    }
    void SequencerEditor::Helper::ProcessFollowLine(SequencerEditor* editor, ImDrawList* drawList) 
    {
        if (nullptr == editor || nullptr == drawList)
        {
            return;
        }
        if (true == editor->HasFlags(FLAGS_HIDE_CURSOR_LINE))
        {
            return;
        }
        ImVec2 mousePos = editor->_mousePos;
        bool isContain  = editor->_canvasRect.Contains(mousePos);
        if (true == isContain)
        {
            DrawFollowLine(editor, drawList);
        }
    }
    void SequencerEditor::Helper::DrawCanvasUpperRect(SequencerEditor* editor, ImDrawList* drawList) 
    {
        ImVec2 min   = editor->_canvasRectUpper.Min;
        ImVec2 max   = editor->_canvasRectUpper.Max;
        ImU32  color = editor->ReflectFields->UpperBgColor[0];
        drawList->AddRectFilled(min, max, color);
    }
    void SequencerEditor::Helper::DrawCanvasLowerRect(SequencerEditor* editor, ImDrawList* drawList) 
    {
        float minFrame = editor->GetMinFrame();
        float maxFrame = editor->GetMaxFrame();
        bool  isValid  = (maxFrame >= minFrame);

        ImVec2 min = editor->_canvasRectLower.Min;
        ImVec2 max = editor->_canvasRectLower.Max;
        ImU32  color = isValid ? editor->ReflectFields->LowerInvaildBgColor[0] : editor->ReflectFields->InvalidColor[0];
        drawList->AddRectFilled(min, max, color);
        if (false == isValid)
        {
            drawList->AddText(min + ImVec2(5.0f, 0), ImColor(1.0f, 1.0f, 1.0f, 1.0f), "Invalid Min-Max Frame");
        }
    }
    void SequencerEditor::Helper::DrawUnitLines(SequencerEditor* editor, ImDrawList* drawList, int frame, const ImVec2& start, const ImVec2& middle, const ImVec2& end)
    {
        ImU32 thickLineColor    = editor->ReflectFields->ThickLineColor[0];
        ImU32 thinLineColor     = editor->ReflectFields->ThinLineColor[0];
        std::string frameText   = std::to_string(frame);

        drawList->AddText(start + ImVec2(5.0f, 0), ImColor(1.0f, 1.0f, 1.0f, 1.0f), frameText.c_str());

        // Draw Thick Line (Canvas Upper)
        drawList->AddLine(start, middle, thickLineColor, 2.0f);

        // Draw Thin Line (Canvas Lower)
        drawList->AddLine(middle, end, thinLineColor, 1.0f);
    }
    void SequencerEditor::Helper::DrawMinMaxLines(SequencerEditor* editor, ImDrawList* drawList, const ImVec2& start, const ImVec2& end, int interactState, ImGuiDir direction, float directionHeight) 
    {
        float directionOffset   = (direction == ImGuiDir_Left) ? -1.0f : 1.0f;
        float canvasUpperHeight = editor->_canvasRectUpper.GetHeight() * directionHeight;

        ImU32 color = editor->ReflectFields->MinMaxLineColor[interactState];

        drawList->AddLine(start, end, color, 1.0f);
        drawList->PathLineTo(start);
        drawList->PathLineTo(start + ImVec2(0.0f, canvasUpperHeight));
        drawList->PathLineTo(start + ImVec2(directionOffset * canvasUpperHeight, 0.0f));
        drawList->PathFillConvex(color);
    }
    void SequencerEditor::Helper::DrawCurrentFrameLine(SequencerEditor* editor, ImDrawList* drawList, const ImVec2& start, const ImVec2& end, const ImRect& rect, int interactState)
    {
        ImVec2 points[5] = {rect.GetBL(), rect.GetTL(), rect.GetTR(), rect.GetBR(), start};
        PathLines(drawList, points, 5);
        // 최종 그리기
        ImU32 color = editor->ReflectFields->CurFrameLineColor[interactState];
        drawList->PathFillConvex(color);
        drawList->AddLine(start, end, color, 2.0f);
    }
    void SequencerEditor::Helper::DrawContext(SequencerEditor* editor, ImDrawList* drawList, EventContext* context,
                                              int groupIndex, const ImRect& rect, const ImVec2& offset)
    {
        if (nullptr == editor || nullptr == drawList || nullptr == context)
        {
            return;
        }

        UINT             id     = context->ID;
        float            time   = context->Time;
        std::string_view label  = context->Label;

        float  unitSize         = editor->_unitToScaledSize;
        float  linePerFrame     = (float)editor->GetLineUnit();
        bool   isSelected       = (id == editor->_seletedContextID);
        ImU32  color            = isSelected ? editor->ReflectFields->ContextColor[3] :editor->ReflectFields->ContextColor[0];
        ImVec2 center           = rect.GetCenter();
        
        if (false == editor->HasFlags(FLAGS_HIDE_CONTEXT_LINE))
        {
            if (editor->HasFlags(FLAGS_DRAW_CONTEXT_LINE_VERTICAL))
            {
                ImVec2 start = ImVec2(center.x, editor->_canvasRectLower.Min.y);
                ImVec2 end   = ImVec2(center.x, editor->_canvasRectLower.Max.y);
                drawList->AddLine(start, end, color, 1.0f);
            }
            else
            {
                ImVec2 start = offset + ImVec2((float)groupIndex * unitSize, 0.0f);
                ImVec2 end   = offset + ImVec2(((float)groupIndex + linePerFrame) * unitSize, 0.0f);
                drawList->AddLine(start, end, color, 1.0f);
            }
        }

        // mainRect
        float  halfWidth = rect.GetWidth() * 0.5f;
        ImVec2 points[4] = {center + ImVec2(0.0f, halfWidth),
                            center + ImVec2(-halfWidth, 0.0f),
                            center + ImVec2(0.0f, -halfWidth),
                            center + ImVec2(halfWidth, 0.0f)};

        PathLines(drawList, points, 4);
        drawList->PathFillConvex(color);
        if (false == editor->HasFlags(FLAGS_HIDE_CONTEXT_LABEL))
        {
            // labelRect
            ImVec2 textSize   = ImGui::CalcTextSize(label.data());
            ImVec2 textOffset = ImVec2(textSize.x, textSize.y * 0.5f) * 1.2f;
            ImRect labelRect  = ImRect(center + ImVec2(0.0f, -textOffset.y), center + textOffset);
            drawList->AddRectFilled(labelRect.Min, labelRect.Max, color);
            if (true == isSelected)
            {
                ImVec2 outlineOffset = ImVec2(1.0f, 1.0f);
                drawList->AddRect(labelRect.Min - outlineOffset, labelRect.Max + outlineOffset, color, 2.0f,
                                  ImDrawFlags_RoundCornersAll, 3.0f);
            }
            ImVec2 textPoint = labelRect.Min + (ImVec2(textOffset.x - textSize.x, 0.0f) * 0.5f);
            drawList->AddText(textPoint, IM_COL32(0, 0, 0, 255), label.data());
        }
    }
    void SequencerEditor::Helper::DrawFollowLine(SequencerEditor* editor, ImDrawList* drawList) 
    {
        auto& dragHandler  = editor->_dragHandler;
        float   offsetX    = editor->_viewToScaledPos.x;
        float   mouseFrame = editor->_mouseFrame;
        float   minFrame   = editor->GetMinFrame();
        float   maxFrame   = editor->GetMaxFrame();
        ImRect& canvas     = editor->_canvasRectLower;
        float   lineX      = offsetX + mouseFrame * editor->_unitToScaledSize;

        ImVec2 start = ImVec2(lineX, 0.0f) + canvas.Min;
        ImVec2 end   = start + ImVec2(0.0f, canvas.GetHeight());

        std::string frameText     = std::format("{:.3f}", mouseFrame);
        ImU32       textColor     = ImColor(1.0f, 1.0f, 1.0f, 1.0f);
        ImU32       lineColor     = editor->ReflectFields->FollowLineColor[0];
        float       lineThickness = 2.0f;
        if (false == dragHandler.IsDragging())
        {
            if (mouseFrame < minFrame || mouseFrame > maxFrame)
            {
                textColor     =  editor->ReflectFields->InvalidColor[0];
                lineColor     =  editor->ReflectFields->InvalidColor[0];
                lineThickness = 4.0f;
            }
            drawList->AddLine(start, end, lineColor, lineThickness);
        }
        drawList->AddText(start + ImVec2(5.0f, 0), textColor, frameText.c_str());
    }
    void SequencerEditor::Helper::PathLines(ImDrawList* drawList, ImVec2* points, size_t pointCount) 
    {
        for (size_t i = 0; i < pointCount; ++i)
        {
            drawList->PathLineTo(points[i]);
        }
    }
} // namespace Timeline