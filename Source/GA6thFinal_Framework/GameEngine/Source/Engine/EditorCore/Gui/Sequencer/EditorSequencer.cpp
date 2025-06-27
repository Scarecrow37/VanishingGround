#include "pch.h"
#include "EditorSequencer.h"
#include "Engine/TimelineCore/TimelineSystem.h"

EditorSequencer::EditorSequencer() 
    : _system(nullptr)
    , _useSnapping(false)
    , _cursorFrame(0.0f)
    , _indicateFrame(0.0f)
    , _canvasUpperHeight(10.0f)
    , _viewLerpTarget(1.0f)
    , _zoomMin(0.05f)
    , _zoomMax(10.0f)
    , _viewPosition(ImVec2(0, 0))
    , _zoomPosition(ImVec2(0, 0))
{
}

EditorSequencer::~EditorSequencer() 
{
}

void EditorSequencer::Show(bool debug)
{
    ImGui::PushID(this);

    //DrawToolBar();

    if(Begin())
    {
        DrawCanvas(debug);
    }
    End();

    ImGui::PopID();
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

    if (_system)
    {
        ContextMenu();
    }
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

struct InteractionData
{
    ImVec2 Start;
    ImVec2 End;
};

void EditorSequencer::DrawCanvas(bool debug)
{
    auto& io                = ImGui::GetIO();
    auto* drawList          = ImGui::GetWindowDrawList();
   
    const bool   isContain  = _canvasRect.Contains(io.MousePos);

    const ImVec2 mousePos   = io.MousePos - _canvasRect.Min;    // mouse position relative to the canvas
    ImVec2       cursorPos  = _cursorPosition + _canvasRect.Min;
    const ImVec2 canvasSize = _canvasRect.GetSize();

    const float curFrame    = GetCurrentFrame();              // current frame in the timeline
    const float minFrame    = GetMinFrame();            // minimum frame in the timeline
    const float maxFrame    = GetMaxFrame();                    // maximum frame in the timeline

    const int   lineUnit    = GetLineUnit();
    const float unitDistane = ReflectFields->UnitSize * ReflectFields->ViewScale;
    const float offsetX     = _viewPosition.x * ReflectFields->ViewScale;
    const float startX      = fmodf(offsetX, unitDistane);

    bool isSnapped = false;
    ImVec2 snapPos = ImVec2(0.0f, 0.0f);

    std::vector<InteractionData> Interactions;
    Interactions.reserve(20);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Draw Background
    //////////////////////////////////////////////////////////////////////////////////////////////////
    ImVec2 validRectMin = ImVec2(offsetX + (minFrame * unitDistane), 0.0f) + _canvasRectLower.Min;
    ImVec2 validRectMax = ImVec2(offsetX + (maxFrame * unitDistane), canvasSize.y) + _canvasRectLower.Min;
    drawList->AddRectFilled(validRectMin, validRectMax, ReflectFields->LowerVaildBgColor[0]);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Draw Line
    //////////////////////////////////////////////////////////////////////////////////////////////////

    // Draw Unit Lines
    int unitFrame = GetFrameFromXToInt(-_viewPosition.x, ReflectFields->UnitSize);
    for (float x = startX; x < canvasSize.x; x += unitDistane, ++unitFrame)
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
        Interactions.emplace_back(middle, end);
    }

    // Draw Min, Max Lines
    if(_system)
    {
        ImVec2 start = ImVec2(validRectMin.x, _canvasRect.Min.y);
        ImVec2 end   = ImVec2(start.x, _canvasRectLower.Max.y);

        ImRect dragRect(start + ImVec2(-2.0f, 0.0f), end + ImVec2(2.0f, 0.0f));
        int    state = dragHandler.BeginDragState("MinFrameLine", dragRect, cursorPos, ImGuiMouseButton_Left, EditorDragState::DRAG_FLAG_NONE);
        switch (state)
        {
        case EditorDragState::DRAG_STATE_NONE:
            Interactions.emplace_back(start, end);
            break;
        case EditorDragState::DRAG_STATE_START:
            ChangeMinFrame(minFrame);
            break;
        case EditorDragState::DRAG_STATE_DRAGGING:
            _system->SetMinFrame(_cursorFrame);
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
        int    state = dragHandler.BeginDragState("MaxFrameLine", dragRect, cursorPos, ImGuiMouseButton_Left, EditorDragState::DRAG_FLAG_NONE);
        switch (state)
        {
        case EditorDragState::DRAG_STATE_NONE:
            Interactions.emplace_back(start, end);
            break;
        case EditorDragState::DRAG_STATE_START:
            ChangeMaxFrame(maxFrame);
            break;
        case EditorDragState::DRAG_STATE_DRAGGING:
            _system->SetMaxFrame(_cursorFrame);
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
        if (EditorDragState::DRAG_STATE_NONE == dragHandler.GetDragState(id))
        {   // 스탬프바 드래깅 중이 아닐 때만 상호작용을 한다.
            Interactions.emplace_back(start, end);
        }
        { // Draw Point Rect
            float  tipDepth = 0.3f;
            ImVec2 size(_canvasUpperHeight * 0.5f, _canvasUpperHeight);
            ImVec2 pos(start.x - (size.x * 0.5f), start.y - size.y);
            ImRect rect(pos, pos + (size * ImVec2(1.0f, 1.0f - tipDepth)));
            ImRect pointRect(pos, pos + size);

            bool isHovered      = pointRect.Contains(io.MousePos);
            bool isMouseDown    = ImGui::IsMouseDown(ImGuiMouseButton_Left);
            bool isMouseClicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
            
            interacted = GetInteractionState(rect);

            int dragState = dragHandler.BeginDragState(id, pointRect, cursorPos, ImGuiMouseButton_Left);
            bool isDragging = dragHandler.IsDragging(dragState);
            if (true == isDragging)
            {
                float frame = ImClamp(_indicateFrame, minFrame, maxFrame);
                _system->SetCurrentFrame(frame);
            }
            ImVec2 points[5] = {rect.GetBL(), rect.GetTL(), rect.GetTR(), rect.GetBR(), start };
            PathLines(drawList, points, 5);
            // 최종 그리기
            drawList->PathFillConvex(ReflectFields->CurFrameLineColor[interacted]);
        }
        drawList->AddLine(start, end, ReflectFields->CurFrameLineColor[interacted], 2.0f);
    }

    // Draw Notify
    if (_system)
    {
        const auto& notifyList = _system->GetTimelineNotifyList();
        for (int i = 0; i < notifyList.size(); ++i)
        {
            auto* notify = notifyList[i];
            if (nullptr == notify)
            {
                continue;
            }
            std::string_view label    = notify->Label;
            UINT             id       = notify->ID;
            ImVec2           textSize = ImGui::CalcTextSize(label.data());

            float  lenght  = textSize.y;
            float  padding = lenght * 2;
            ImVec2 point   = ImVec2(offsetX + (notify->Time * unitDistane), padding) + _canvasRectLower.Min;
            ImRect rect    = ImRect(point - ImVec2(lenght, lenght), point + ImVec2(lenght, lenght));
            int    state =
                dragHandler.BeginDragState(id, rect, cursorPos, ImGuiMouseButton_Left, EditorDragState::DRAG_FLAG_LOCK);
            switch (state)
            {
            case EditorDragState::DRAG_STATE_NONE:
                Interactions.emplace_back(point, point);
                break;
            case EditorDragState::DRAG_STATE_START:
                break;
            case EditorDragState::DRAG_STATE_DRAGGING:
                _system->ChangeNotifyTime(id, _indicateFrame);
                break;
            default:
                break;
            }

            bool isHovered   = rect.Contains(io.MousePos);
            bool isRBMouseUp = ImGui::IsMouseReleased(ImGuiMouseButton_Right);
            bool isDragging  = dragHandler.IsDragging();
            ImGui::PushID(notify);
            if (false == isDragging && true == isHovered && true == isRBMouseUp)
            {
                ImGui::OpenPopup("NotifyPopup");
            }
            if (ImGui::BeginPopup("NotifyPopup"))
            {
                float time = notify->Time;
                UINT  id   = notify->ID;
                if (true == debug)
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

            float  offset    = lenght * 0.5f;
            ImVec2 txtOffset = ImVec2(textSize.x, textSize.y * 0.5f) * 1.2f;
            ImRect mainRect  = ImRect(point + ImVec2(-offset, -offset), point + ImVec2(offset, offset));
            ImRect labelRect = ImRect(point + ImVec2(0.0f, -txtOffset.y), point + txtOffset);

            bool isValid    = notify->Time < minFrame && notify->Time > maxFrame;
            int  interacted = GetInteractionState(rect);
            UINT color      = isValid ? ReflectFields->InvalidColor[0] : ReflectFields->NotifyColor[interacted];

            // mainRect
            float  halfWidth = mainRect.GetWidth() * 0.5f;
            ImVec2 center    = mainRect.GetCenter();
            ImVec2 points[4] = {center + ImVec2(0.0f, halfWidth), center + ImVec2(-halfWidth, 0.0f),
                                center + ImVec2(0.0f, -halfWidth), center + ImVec2(halfWidth, 0.0f)};
            PathLines(drawList, points, 4);
            drawList->PathFillConvex(color);

            // labelRect
            drawList->AddRectFilled(labelRect.Min, labelRect.Max, color);
            ImVec2 textPoint = labelRect.Min + (ImVec2(txtOffset.x - textSize.x, 0.0f) * 0.5f);
            drawList->AddText(textPoint, IM_COL32(0, 0, 0, 255), label.data());
        }
    }
   

    // ProcessInterction
    for (const auto& line : Interactions)
    {
        if (true == _useSnapping)
        {
            const float snapRange = (unitDistane * (float)lineUnit) * 0.1f /* = SnapFactor*/;
            // Check if the mouse is within the snapping range
            bool snapCheck = io.MousePos.x >= line.Start.x - snapRange && io.MousePos.x <= line.Start.x + snapRange;
            if (true == isContain && true == snapCheck)
            {
                isSnapped = true;
                snapPos   = ImVec2(line.Start.x - _canvasRect.Min.x, mousePos.y);
            }
        }
    }

    // Draw FollowLine
    if (true == isContain)
    {
        ImVec2 linePos     = isSnapped ? snapPos : mousePos;
        float canvasSapceX = -_viewPosition.x + linePos.x / ReflectFields->ViewScale;
        bool anyPopupOpened = ImGui::IsPopupOpen("", ImGuiPopupFlags_AnyPopupId);
        if (false == anyPopupOpened)
        {
            _cursorPosition = linePos;
            _cursorFrame = GetFrameFromXToFloat(canvasSapceX, ReflectFields->UnitSize);
            _indicateFrame = ImClamp(_cursorFrame, minFrame, maxFrame);
        }
        ImVec2 start = ImVec2(offsetX + (_cursorFrame * unitDistane), 0.0f) + _canvasRectLower.Min;
        ImVec2 end   = ImVec2(offsetX + (_cursorFrame * unitDistane), _canvasRectLower.GetHeight()) + _canvasRectLower.Min;

        std::string frameText = std::format("{:.3f}", _cursorFrame);
        if (_cursorFrame >= GetMinFrame() && _cursorFrame <= GetMaxFrame())
        {
            drawList->AddText(start + ImVec2(5.0f, 0), ImColor(1.0f, 1.0f, 1.0f, 1.0f), frameText.c_str());
            drawList->AddLine(start, end, ReflectFields->FollowLineColor[0], 2.0f);
        }
        else
        {
            drawList->AddText(start + ImVec2(5.0f, 0), ReflectFields->InvalidColor[0], frameText.c_str());
            drawList->AddLine(start, end, ReflectFields->InvalidColor[0], 4.0f);
        }
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
        _zoomPosition   = io.MousePos - _frameRect.Min;
    }

    float& viewScale = ReflectFields->ViewScale;
    if (false == std::isnan(_zoomPosition.x))
    {
        ImVec2 mouseWPosPre, mouseWPosPost;
        mouseWPosPre.x  = _zoomPosition.x / viewScale;
        viewScale       = ImLerp(viewScale, _viewLerpTarget, ReflectFields->ViewLerpScale);
        mouseWPosPost.x = _zoomPosition.x / viewScale;
        _viewPosition += mouseWPosPost - mouseWPosPre;
    }

    return isZooming;
}

bool EditorSequencer::CanvasDragging()
{
    ImGuiIO&     io     = ImGui::GetIO();
    const char*  id     = "CanvasFrame";
    ImVec2 mousePos     = _cursorPosition + _canvasRect.Min;
    bool   isContain    = _canvasRect.Contains(io.MousePos);
    if (false == isContain)
    {
        return false;
    }

    int flags = EditorDragState::DRAG_FLAG_LOCK | EditorDragState::DRAG_FLAG_MOVED;
    int state = dragHandler.BeginDragState(id, _canvasRect, mousePos, ImGuiMouseButton_Right, flags);
    bool isDragging = (state == EditorDragState::DRAG_STATE_DRAGGING);
    if (true == isDragging)
    {
        ImVec2 mouseDelta = io.MouseDelta;
        bool isMoved = (mouseDelta.x != 0.0f);
        if (true == isMoved)
        {
            _viewPosition += ImVec2(mouseDelta.x / ReflectFields->ViewScale, 0.0f);
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

    int dragState = dragHandler.GetDragState("CanvasFrame");
    if (true == isMouseRBUp && true == isLowerContain && EditorDragState::DRAG_STATE_NONE == dragState)
    {
        ImGui::OpenPopup("LowerContextMenu");
    }

    if (ImGui::BeginPopup("LowerContextMenu"))
    {
        if (ImGui::BeginMenu("Add Notify"))
        {
            auto& table = _system->GetInstanceConstructors();
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
    _eventQueue.push([this, time, label, typeNameID]() {
        UmCommandManager.Do<Command::Sequencer::AddNotify>(_system, time, label, typeNameID);
    });
}

void EditorSequencer::RemoveNotify(TimelineNotify* notify)
{
    _eventQueue.push([this, notify]() { 
        dragHandler.RemoveDragState(notify->ID);
        UmCommandManager.Do<Command::Sequencer::RemoveNotify>(_system, notify); 
        });
}

void EditorSequencer::ChangeNotify(TimelineNotify* notify, float time, std::string_view label, std::string_view typeNameID) 
{
    _eventQueue.push([this, notify, time, label, typeNameID]() {
        UmCommandManager.Do<Command::Sequencer::ChangeNotify>(_system, notify, time, label, typeNameID);
    });
}

void EditorSequencer::ChangeMinFrame(float frame) 
{
    _eventQueue.push([this, frame]() {
        UmCommandManager.Do<Command::Sequencer::ChangeMinFrame>(_system, frame); 
        });
}

void EditorSequencer::ChangeMaxFrame(float frame) 
{
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
    return ImVec2(_viewPosition + pos) * ReflectFields->ViewScale;
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
