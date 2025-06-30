#include "pch.h"
#include "EditorSequencer.h"
#include "Engine/TimelineCore/TimelineSystem.h"

EditorSequencer::EditorSequencer() 
    : _system(nullptr)
    , _flags(0)
    , _isSnapped(false)
    , _mouseFrame(0.0f)
    , _indicateFrame(0.0f)
    , _canvasUpperHeight(10.0f)
    , _viewLerpTarget(1.0f)
    , _zoomMin(0.05f)
    , _zoomMax(10.0f)
    , _viewPos(ImVec2(0, 0))
    , _zoomPos(ImVec2(0, 0))
{
}

EditorSequencer::~EditorSequencer() 
{
}

void EditorSequencer::Show()
{
    if (nullptr == _system)
    {
        return;
    }
    ImGui::PushID(this);

    //DrawToolBar();

    if(Begin())
    {
        DrawCanvas();
    }
    End();

    ImGui::PopID();
}

void EditorSequencer::SetSystem(std::shared_ptr<TimelineSystem> system) 
{
    // 이미 같은 시스템이 설정되어 있다면 아무 작업도 하지 않음
    if (system != _system)
    {  
         _system = system;
        SetSelectedNotifyID(0);
        _dragHandler.ClearDragState();
    }
}

void EditorSequencer::ShowDebugData() 
{
    ImGui::DragFloat("Zoom Scale", &ReflectFields->ViewScale, 0.01f, _zoomMin, _zoomMax);
    ImGui::DragFloat2("View Position", &_viewPos.x, 0.1f, -10000.0f, 10000.0f);
    ImGui::Text("Selected Notify ID: %d", _seletedNotifyID);
    if (ImGui::TreeNodeEx("Mouse", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Mouse Position: (%.2f, %.2f)", _mousePos.x, _mousePos.y);
        ImGui::Text("Canvas Mouse Position: (%.2f, %.2f)", _canvasMousePos.x, _canvasMousePos.y);
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("Snap", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Snap Position: (%.2f, %.2f)", _snapPos.x, _snapPos.y);
        ImGui::Text("Canvas Snap Position: (%.2f, %.2f)", _canvasSnapPos.x, _canvasSnapPos.y);
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("Indicate", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Indicate Position: (%.2f, %.2f)", _indicatePos.x, _indicatePos.y);
        ImGui::Text("Canvas Indicate Position: (%.2f, %.2f)", _canvasIndicatePos.x, _canvasIndicatePos.y);
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

bool EditorSequencer::Begin()
{
    ImGuiIO&    io       = ImGui::GetIO();
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // Update Rect
    ImVec2 windowPos  = ImGui::GetCursorScreenPos();
    ImVec2 frameSize  = ImGui::GetContentRegionAvail();
    ImVec2 canvasSize = ImVec2(frameSize.x, frameSize.y);

    if (canvasSize.y < 300.0f)
    {
        ImGui::Dummy(ImVec2(canvasSize.x, 300.0f)); // Ensure the canvas is drawn with the correct size
        canvasSize.y = 300.0f;
    }

    _frameRect  = ImRect(windowPos, windowPos + frameSize);
    _canvasRect = ImRect(windowPos, windowPos + canvasSize);

    ContextMenu();
    WheelZooming();
    CanvasDragging();

    ImGui::PushClipRect(_canvasRect.Min, _canvasRect.Max, true);

    _canvasUpperHeight  = 20.0f;
    _canvasRectUpper    = ImRect(_canvasRect.Min, ImVec2(_canvasRect.Max.x, _canvasRect.Min.y + _canvasUpperHeight));
    _canvasRectLower    = ImRect(ImVec2(_canvasRect.Min.x, _canvasRect.Min.y + _canvasUpperHeight), _canvasRect.Max);
    drawList->AddRectFilled(_canvasRectUpper.Min, _canvasRectUpper.Max, ReflectFields->UpperBgColor[0]);

    if (GetMaxFrame() < GetMinFrame())
    {
        drawList->AddText(_canvasRectLower.Min, ImColor(1.0f, 1.0f, 1.0f, 1.0f), "Invalid Min-Max Frame");
        drawList->AddRectFilled(_canvasRectLower.Min, _canvasRectLower.Max, ReflectFields->InvalidColor[0]);
        return false;
    }
    else
    {
        drawList->AddRectFilled(_canvasRectLower.Min, _canvasRectLower.Max, ReflectFields->LowerInvaildBgColor[0]);
    }
    return true;
}

void EditorSequencer::End() 
{
    ImGui::PopClipRect();

    if (false == _eventQueue.empty())
    {
        _eventQueue.front()();
        _eventQueue.pop();
    }
}

void EditorSequencer::DrawToolBar() 
{
    ImGui::PushStyleColor(ImGuiCol_Button, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f); // 모서리 라운딩 정도
    if (ImGui::Button(EditorIcon::ICON_FILE_SAVE))
    {
        ReflectFields->SerializedData = _system->SerializedReflectFields();
    }
    if (ImGui::Button(EditorIcon::ICON_PLAY))
    {
        _system->Play();
    }
    ImGui::SameLine();
    if (ImGui::Button(EditorIcon::ICON_PAUSE))
    {
        _system->Pause();
    }
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

void EditorSequencer::DrawCanvas()
{
    _isSnapped = false;
    _interactionList.clear();

    auto& io = ImGui::GetIO();
    auto* drawList = ImGui::GetWindowDrawList();
   
    const bool   isContain  = _canvasRect.Contains(io.MousePos);

    _mousePos               = io.MousePos;                   
    _canvasMousePos         = io.MousePos - _canvasRect.Min; 
    _snapPos                = ImVec2(0.0f, 0.0f);
    _canvasSnapPos          = ImVec2(0.0f, 0.0f);
    _viewToScaledPos        = _viewPos * ReflectFields->ViewScale;
    _unitToScaledSize       = ReflectFields->UnitSize * ReflectFields->ViewScale;

    const ImVec2 canvasSize = _canvasRect.GetSize();

    const float curFrame    = GetCurrentFrame();              // current frame in the timeline
    const float minFrame    = GetMinFrame();            // minimum frame in the timeline
    const float maxFrame    = GetMaxFrame();                    // maximum frame in the timeline

    const int   lineUnit    = GetLineUnit();
   
    
    const float  startX     = fmodf(_viewToScaledPos.x, _unitToScaledSize);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Draw Background
    //////////////////////////////////////////////////////////////////////////////////////////////////
    ImVec2 validRectMin = ImVec2(_viewToScaledPos.x + (minFrame * _unitToScaledSize), 0.0f) + _canvasRectLower.Min;
    ImVec2 validRectMax = ImVec2(_viewToScaledPos.x + (maxFrame * _unitToScaledSize), canvasSize.y) + _canvasRectLower.Min;
    drawList->AddRectFilled(validRectMin, validRectMax, ReflectFields->LowerVaildBgColor[0]);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Draw Line
    //////////////////////////////////////////////////////////////////////////////////////////////////

    // Draw Unit Lines
    int unitFrame = GetFrameFromXToInt(-_viewPos.x, ReflectFields->UnitSize);
    for (float x = startX; x < canvasSize.x; x += _unitToScaledSize, ++unitFrame)
    {
        if (unitFrame % lineUnit != 0)
        {
            continue;
        }
        ImVec2 start  = ImVec2(x, 0.0f) + _canvasRectUpper.Min;
        ImVec2 middle = ImVec2(x, 0.0f) + _canvasRectLower.Min;
        ImVec2 end    = ImVec2(x, canvasSize.y) + _canvasRectLower.Min;

        std::string frameText = std::to_string(unitFrame);
        drawList->AddText(start + ImVec2(5.0f, 0), ImColor(1.0f, 1.0f, 1.0f, 1.0f), std::to_string(unitFrame).c_str());
        drawList->AddLine(start, middle, ReflectFields->ThickLineColor[0], 2.0f);
        drawList->AddLine(middle, end, ReflectFields->ThinLineColor[0], 1.0f);
        _interactionList.emplace_back(middle, end);
    }

    // Draw Min, Max Lines
    if(_system)
    {
        ImVec2 start = ImVec2(validRectMin.x, _canvasRect.Min.y);
        ImVec2 end   = ImVec2(start.x, _canvasRectLower.Max.y);

        ImRect dragRect(start + ImVec2(-2.0f, 0.0f), end + ImVec2(2.0f, 0.0f));
        bool   canDrag   = HasFlags(FLAGS_USE_DRAG_MIN_MAX_FRAME);
        int    dragState = EditorDragState::DRAG_STATE_NONE;
        if (true == canDrag)
        {
            dragState = _dragHandler.BeginDragState("MinFrameLine", dragRect, _indicatePos);
        }
        switch (dragState)
        {
        case EditorDragState::DRAG_STATE_NONE:
            _interactionList.emplace_back(start, end);
            break;
        case EditorDragState::DRAG_STATE_START:
            ChangeMinFrame(minFrame);
            break;
        case EditorDragState::DRAG_STATE_DRAGGING:
            _system->SetMinFrame(_mouseFrame);
            break;
        default:
            break;
        }

        int interacted = GetInteractionState(dragRect);
        drawList->AddLine(start, end, ReflectFields->MinMaxLineColor[interacted], 1.0f);
        drawList->PathLineTo(start);
        drawList->PathLineTo(start + ImVec2(0.0f, _canvasRectUpper.GetHeight() * 0.7f));
        drawList->PathLineTo(start + ImVec2(_canvasRectUpper.GetHeight() * 0.7f, 0.0f));
        drawList->PathFillConvex(ReflectFields->MinMaxLineColor[interacted]);
    }
    if (_system)
    {
        ImVec2 start = ImVec2(validRectMax.x, _canvasRect.Min.y);
        ImVec2 end   = ImVec2(start.x, _canvasRectLower.Max.y);

        ImRect dragRect(start + ImVec2(-2.0f, 0.0f), end + ImVec2(2.0f, 0.0f));
        bool   canDrag   = HasFlags(FLAGS_USE_DRAG_MIN_MAX_FRAME);
        int    dragState = EditorDragState::DRAG_STATE_NONE;
        if (true == canDrag)
        {
            dragState = _dragHandler.BeginDragState("MaxFrameLine", dragRect, _indicatePos);
        }
        switch (dragState)
        {
        case EditorDragState::DRAG_STATE_NONE:
            _interactionList.emplace_back(start, end);
            break;
        case EditorDragState::DRAG_STATE_START:
            ChangeMaxFrame(maxFrame);
            break;
        case EditorDragState::DRAG_STATE_DRAGGING:
            _system->SetMaxFrame(_mouseFrame);
            break;
        default:
            break;
        }

        int interacted = GetInteractionState(dragRect);
        drawList->AddLine(start, end, ReflectFields->MinMaxLineColor[interacted], 1.0f);
        drawList->PathLineTo(start);
        drawList->PathLineTo(start + ImVec2(0.0f, _canvasRectUpper.GetHeight() * 0.7f));
        drawList->PathLineTo(start + ImVec2(-_canvasRectUpper.GetHeight() * 0.7f, 0.0f));
        drawList->PathFillConvex(ReflectFields->MinMaxLineColor[interacted]);
    }

    // Draw Current Frame Line
    if (_system)
    {
        const char* id = "StampBar";

        ImVec2 linePos     = ImVec2(curFrame * ReflectFields->UnitSize, 0.0f);
        ImVec2 canvasSapce = PositionToCanvasSapce(linePos);

        ImVec2 start = canvasSapce + _canvasRectLower.Min;
        ImVec2 end   = start + ImVec2(0.0f, canvasSize.y);

        int interacted = 0;
        if (EditorDragState::DRAG_STATE_NONE == _dragHandler.GetDragState(id))
        {   // 스탬프바 드래깅 중이 아닐 때만 상호작용을 한다.
            _interactionList.emplace_back(start, end);
        }
        {   // Draw Point Rect
            float  tipDepth = 0.3f;
            ImVec2 size(_canvasUpperHeight * 0.5f, _canvasUpperHeight);
            ImVec2 pos(start.x - (size.x * 0.5f), start.y - size.y);
            ImRect rect(pos, pos + (size * ImVec2(1.0f, 1.0f - tipDepth)));
            ImRect pointRect(pos, pos + size);

            bool isHovered      = pointRect.Contains(_mousePos);
            bool isMouseDown    = ImGui::IsMouseDown(ImGuiMouseButton_Left);
            bool isMouseClicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
            
            interacted = GetInteractionState(rect);

            bool canDrag    = HasFlags(FLAGS_USE_DRAG_FRAME_LINE);
            if (true == canDrag)
            {
                int  dragState  = _dragHandler.BeginDragState(id, pointRect, _indicatePos);
                bool isDragging = _dragHandler.IsDragging(dragState);
                if (true == isDragging)
                {
                    float frame = ImClamp(_indicateFrame, minFrame, maxFrame);
                    _system->SetCurrentFrame(frame);
                }
            }
            ImVec2 points[5] = {rect.GetBL(), rect.GetTL(), rect.GetTR(), rect.GetBR(), start };
            PathLines(drawList, points, 5);
            // 최종 그리기
            drawList->PathFillConvex(ReflectFields->CurFrameLineColor[interacted]);
        }
        drawList->AddLine(start, end, ReflectFields->CurFrameLineColor[interacted], 2.0f);
    }

    // Draw Notify
    const auto& notifyList = _system->GetTimelineNotifyList();
    std::deque<TimelineNotify*> drawNotifyQueue;
    for (int i = 0; i < notifyList.size(); ++i)
    {
        if (nullptr != notifyList[i])
        {
            if (_seletedNotifyID == notifyList[i]->ID) drawNotifyQueue.push_back(notifyList[i]);
            else drawNotifyQueue.push_front(notifyList[i]);
        }
    }
    for (int i = 0; i < drawNotifyQueue.size(); ++i)
    {
        auto* notify    = drawNotifyQueue[i];
        float  lenght   = 7.0f;
        float  paddingY = 40.0f; 
        ImVec2 point    = ImVec2(_viewToScaledPos.x + (notify->Time * _unitToScaledSize), paddingY) + _canvasRectLower.Min;
        ImRect rect     = ImRect(point - ImVec2(lenght, lenght), point + ImVec2(lenght, lenght));

        DrawNotify(drawList, notify, rect);
        DragNotify(notify, rect);
        PopupNotify(notify, rect);
    }

    // ProcessInterction
    for (const auto& line : _interactionList)
    {
        if (true == HasFlags(FLAGS_USE_SNAP))
        {
            const float snapRange = (_unitToScaledSize * (float)lineUnit) * 0.1f /* = SnapFactor*/;
            // Check if the mouse is within the snapping range
            bool snapCheck = _mousePos.x >= line.Start.x - snapRange && _mousePos.x <= line.Start.x + snapRange;
            if (true == isContain && true == snapCheck)
            {
                _isSnapped     = true;
                _snapPos       = ImVec2(line.Start.x, _mousePos.y);
                _canvasSnapPos = _snapPos - _canvasRect.Min;
            }
        }
    }

    // Draw FollowLine
    if (true == isContain)
    {
        _indicatePos       = _isSnapped ? _snapPos : _mousePos;
        _canvasIndicatePos = _isSnapped ? _canvasSnapPos : _canvasMousePos;

        float canvasSapceX   = -_viewPos.x + _canvasIndicatePos.x / ReflectFields->ViewScale;
        bool anyPopupOpened = ImGui::IsPopupOpen("", ImGuiPopupFlags_AnyPopupId);
        if (false == anyPopupOpened)
        {
            _mouseFrame = GetFrameFromXToFloat(canvasSapceX, ReflectFields->UnitSize);
            _indicateFrame = ImClamp(_mouseFrame, minFrame, maxFrame);
        }
        float  lineX = _viewToScaledPos.x + (_mouseFrame * _unitToScaledSize);
        ImVec2 start = ImVec2(lineX, 0.0f) + _canvasRectLower.Min;
        ImVec2 end   = start + ImVec2(0.0f, _canvasRectLower.GetHeight());

        std::string frameText = std::format("{:.3f}", _mouseFrame);
        ImU32 textColor = ImColor(1.0f, 1.0f, 1.0f, 1.0f);
        ImU32 lineColor = ReflectFields->FollowLineColor[0];
        float lineThickness = 2.0f;
        if (false == _dragHandler.IsDragging())
        {
            if (_mouseFrame < GetMinFrame() || _mouseFrame > GetMaxFrame())
            {
                textColor     = ReflectFields->InvalidColor[0];
                lineColor     = ReflectFields->InvalidColor[0];
                lineThickness = 4.0f;
            }
            drawList->AddLine(start, end, lineColor, lineThickness);
        }
        drawList->AddText(start + ImVec2(5.0f, 0), textColor, frameText.c_str());
    }
}

bool EditorSequencer::WheelZooming() 
{
    ImGuiIO& io           = ImGui::GetIO();
    bool     isContain    = _canvasRect.Contains(io.MousePos);
    bool     isMouseValid = ImGui::IsMousePosValid();
    bool     isWheelMoved = (io.MouseWheel < -FLT_EPSILON || io.MouseWheel > FLT_EPSILON);
    bool     isZooming    = isContain && isMouseValid && isWheelMoved;

    if (true == isZooming)
    {
        if (io.MouseWheel < -FLT_EPSILON)
        {
            _viewLerpTarget *= 0.9f * ReflectFields->ZoomScale;
        }

        if (io.MouseWheel > FLT_EPSILON)
        {
            _viewLerpTarget *= 1.1f * ReflectFields->ZoomScale;
        }
        _viewLerpTarget = ImClamp(_viewLerpTarget, _zoomMin, _zoomMax);
        _zoomPos = io.MousePos - _frameRect.Min;
    }

    float& viewScale = ReflectFields->ViewScale;
    if (false == std::isnan(_zoomPos.x))
    {
        ImVec2 mouseWPosPre, mouseWPosPost;
        mouseWPosPre.x  = _zoomPos.x / viewScale;
        viewScale       = ImLerp(viewScale, _viewLerpTarget, ReflectFields->ViewLerpScale);
        mouseWPosPost.x = _zoomPos.x / viewScale;
        _viewPos += mouseWPosPost - mouseWPosPre;
    }

    return isZooming;
}

bool EditorSequencer::CanvasDragging()
{
    ImGuiIO&     io = ImGui::GetIO();
    const char*  id = "CanvasFrame";
    bool isContain  = _canvasRect.Contains(_mousePos);
    if (false == isContain)
    {
        return false;
    }
    int flags = EditorDragState::DRAG_FLAG_LOCK | EditorDragState::DRAG_FLAG_MOVED;
    int state = _dragHandler.BeginDragState(id, _canvasRect, _indicatePos, ImGuiMouseButton_Right, flags);
    bool isDragging = (state == EditorDragState::DRAG_STATE_DRAGGING);
    if (true == isDragging)
    {
        bool isMoved = (io.MouseDelta.x != 0.0f);
        if (true == isMoved)
        {
            _viewPos += ImVec2(io.MouseDelta.x / ReflectFields->ViewScale, 0.0f);
        }
    }
    return isDragging;
}

bool EditorSequencer::ContextMenu()
{
    ImGuiIO& io = ImGui::GetIO();
    bool isMouseRBUp    = ImGui::IsMouseReleased(ImGuiMouseButton_Right);
    bool isMouseLBUp    = ImGui::IsMouseReleased(ImGuiMouseButton_Left);
    bool isContain      = _canvasRect.Contains(io.MousePos);
    bool isUpperContain = _canvasRectUpper.Contains(io.MousePos);
    bool isLowerContain = _canvasRectLower.Contains(io.MousePos);
    bool isMouseMoved   = io.MouseDelta.x >= 1.0f || io.MouseDelta.x <= -1.0f;

    if (true == isMouseRBUp && true == isLowerContain && false == _dragHandler.IsDragging())
    {
        ImGui::OpenPopup("LowerContextMenu");
    }

    if (ImGui::BeginPopup("LowerContextMenu"))
    {
        if (ImGui::BeginMenu("Add Notify"))
        {
            auto& table = TimelineSystem::GetInstanceConstructors();
            for (const auto& [key, func] : table)
            {
                if (ImGui::MenuItem(key.c_str() + 6))
                {
                    AddNotify(_indicateFrame, "Notify", key);
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }
    return true;
}

void EditorSequencer::AddNotify(float time, std::string_view label, std::string_view typeNameID) 
{
    if (nullptr == _system)
        return;
    _eventQueue.push([this, time, label, typeNameID]() {
        UmCommandManager.Do<Command::Sequencer::AddNotify>(_system, time, label, typeNameID);
    });
}

void EditorSequencer::RemoveNotify(TimelineNotify* notify)
{
    if (nullptr == _system)
        return;
    _eventQueue.push([this, notify]() { 
        _dragHandler.RemoveDragState(notify->ID);
        UmCommandManager.Do<Command::Sequencer::RemoveNotify>(_system, notify); 
        });
}

void EditorSequencer::ChangeNotify(TimelineNotify* notify, float time, std::string_view label, std::string_view typeNameID) 
{
    if (nullptr == _system)
        return;
    _eventQueue.push([this, notify, time, label, typeNameID]() {
        UmCommandManager.Do<Command::Sequencer::ChangeNotify>(_system, notify, time, label, typeNameID);
    });
}

void EditorSequencer::ChangeMinFrame(float frame) 
{
    if (nullptr == _system)
        return;
    _eventQueue.push([this, frame]() {
        UmCommandManager.Do<Command::Sequencer::ChangeMinFrame>(_system, frame); 
        });
}

void EditorSequencer::ChangeMaxFrame(float frame) 
{
    if (nullptr == _system)
        return;
    _eventQueue.push([this, frame]() { 
        UmCommandManager.Do<Command::Sequencer::ChangeMaxFrame>(_system, frame); 
        });
}

int EditorSequencer::GetLineUnit() const
{
    int unitFactor = 1;
    if (ReflectFields->ViewScale < 0.1f)
    {
        unitFactor = 10;
    }
    else if (ReflectFields->ViewScale < 0.4f)
    {
        unitFactor = 5;
    }
    else
    {
        unitFactor = 1;
    }
    return unitFactor;
}

int EditorSequencer::GetFrameFromXToInt(float x, float unitSize) const
{
    return static_cast<int>(GetFrameFromXToFloat(x, unitSize));
}

float EditorSequencer::GetFrameFromXToFloat(float x, float unitSize) const
{
    return x / unitSize;
}
ImVec2 EditorSequencer::PositionToCanvasSapce(const ImVec2& pos) const
{
    return ImVec2(_viewPos + pos) * ReflectFields->ViewScale;
}

void EditorSequencer::PathLines(ImDrawList* drawList, ImVec2* points, size_t pointCount) const 
{
    for (size_t i = 0; i < pointCount; ++i)
    {
        drawList->PathLineTo(points[i]);
    }
}

int EditorSequencer::GetInteractionState(const ImRect& rect) const
{
    ImGuiIO& io = ImGui::GetIO();
    bool isHovered = rect.Contains(io.MousePos);
    bool isMouseDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);
    int state = 0;
    if (true == isHovered)
    {
        state = isMouseDown ? 2 : 1; // 2: Pressed, 1: Hovered
    }
    return state;
}

void EditorSequencer::DrawNotify(ImDrawList* drawList, TimelineNotify* notify, const ImRect& mainRect)
{
    UINT             id    = notify->ID;
    float            time  = notify->Time;
    std::string_view label = notify->Label;

    ImVec2 center    = mainRect.GetCenter();
    ImVec2 textSize  = ImGui::CalcTextSize(label.data());
    ImVec2 txtOffset = ImVec2(textSize.x, textSize.y * 0.5f) * 1.2f;
    ImRect labelRect = ImRect(center + ImVec2(0.0f, -txtOffset.y), center + txtOffset);

    bool isValid    = notify->Time < _system->GetMinFrame() && notify->Time > _system->GetMaxFrame();
    bool isSelected = (id == _seletedNotifyID);
    int  interacted = isSelected ? 3 : GetInteractionState(mainRect);
    UINT color      = isValid ? ReflectFields->InvalidColor[0] : ReflectFields->NotifyColor[interacted];

    // mainRect
    float  halfWidth = mainRect.GetWidth() * 0.5f;
    ImVec2 points[4] = {center + ImVec2(0.0f, halfWidth),
                        center + ImVec2(-halfWidth, 0.0f),
                        center + ImVec2(0.0f, -halfWidth),
                        center + ImVec2(halfWidth, 0.0f)};
    PathLines(drawList, points, 4);
    drawList->PathFillConvex(color);

    // labelRect
    drawList->AddRectFilled(labelRect.Min, labelRect.Max, color);
    if (true == isSelected)
    {
        ImVec2 outlineOffset = ImVec2(1.0f, 1.0f);
        drawList->AddRect(
            labelRect.Min - outlineOffset, 
            labelRect.Max + outlineOffset, color, 
            2.0f,
            ImDrawFlags_RoundCornersAll, 
            3.0f);
    }
    ImVec2 textPoint = labelRect.Min + (ImVec2(txtOffset.x - textSize.x, 0.0f) * 0.5f);
    drawList->AddText(textPoint, IM_COL32(0, 0, 0, 255), label.data());
}

void EditorSequencer::DragNotify(TimelineNotify* notify, const ImRect& mainRect) 
{
    if (nullptr == notify)
    {
        return;
    }

    const float time = notify->Time;
    const UINT  id   = notify->ID;
    ImVec2 center    = mainRect.GetCenter();

    bool isSelected  = (notify->ID == _seletedNotifyID);
    int  state       = EditorDragState::DRAG_STATE_NONE;
    if (true == isSelected)
    {
        state = _dragHandler.BeginDragState(id, mainRect, _indicatePos);
    }
    switch (state)
    {
    case EditorDragState::DRAG_STATE_NONE:
        _interactionList.emplace_back(center, center);
        break;
    case EditorDragState::DRAG_STATE_START:
        break;
    case EditorDragState::DRAG_STATE_DRAGGING:
        _system->ChangeNotifyTime(id, _indicateFrame);
        break;
    default:
        break;
    }
}

void EditorSequencer::PopupNotify(TimelineNotify* notify, const ImRect& mainRect) 
{
    if (nullptr == notify)
    {
        return;
    }
    const float time = notify->Time;
    const UINT  id   = notify->ID;

    bool isHovered  = mainRect.Contains(_mousePos);
    bool isSelected = (notify->ID == _seletedNotifyID);

    ImGui::PushID(notify);
    if (true == isHovered)
    {
        bool rUp = ImGui::IsMouseReleased(ImGuiMouseButton_Right);
        bool lUp = ImGui::IsMouseReleased(ImGuiMouseButton_Left);
        if (true == rUp)
        {
            if (false == _dragHandler.IsDragging())
            {
                ImGui::OpenPopup("NotifyPopup");
            }
        }
        if (false == isSelected)
        {
            if (true == rUp || true == lUp)
            {
                SetSelectedNotifyID(id);
            }
        }
    }
    if (ImGui::BeginPopup("NotifyPopup"))
    {
        if (true == HasFlags(FLAGS_DEBUG))
        {
            ImGui::Text("Notify: %f", time);
            ImGui::Text("ID: %d", id);
            ImGui::Separator();
        }
        if (ImGui::MenuItem("Remove Notify"))
        {
            RemoveNotify(notify);
        }
        ImGui::EndPopup();
    }
    ImGui::PopID();
}

float EditorSequencer::GetCurrentFrame() const
{
    return _system ? _system->GetCurrentFrame() : 0.0f;
}

float EditorSequencer::GetMinFrame() const
{
    return _system ? _system->GetMinFrame() : 0.0f;
}

float EditorSequencer::GetMaxFrame() const
{
    return _system ? _system->GetMaxFrame() : 0.0f;
}
