#include "pch.h"
#include "EditorSequencer.h"

EditorSequencer::EditorSequencer() 
    : _system(nullptr)
    , _useSnapping(false)
    , _isDraggingStampBar(false)
    , _isOpenedPopup(false)
    , _canvasRect({})
    , _canvasUpperHeight(10.0f)
    , _viewLerpTarget(1.0f)
    , _zoomMin(0.05f)
    , _zoomMax(1.0f)
    , _viewPosition(ImVec2(0, 0))
    , _ZoomPosition(ImVec2(0, 0))
    , _indicateFrame(0.0f)
{
}

EditorSequencer::~EditorSequencer() 
{
}

void EditorSequencer::Render() 
{
    if (nullptr == _system)
    {
        return;
    }
    ImGui::PushID(_system);

    DrawToolBar();

    if(Begin())
    {
        DrawCanvas();
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

    _frameRect  = ImRect(windowPos, windowPos + frameSize);
    _canvasRect = ImRect(windowPos, windowPos + canvasSize);

    ContextMenu();
    WheelZooming();
    Dragging();

    ImGui::PushClipRect(_canvasRect.Min, _canvasRect.Max, true);

    _canvasUpperHeight  = 20.0f;
    _canvasRectUpper    = ImRect(_canvasRect.Min, ImVec2(_canvasRect.Max.x, _canvasRect.Min.y + _canvasUpperHeight));
    _canvasRectLower    = ImRect(ImVec2(_canvasRect.Min.x, _canvasRect.Min.y + _canvasUpperHeight), _canvasRect.Max);
    drawList->AddRectFilled(_canvasRectUpper.Min, _canvasRectUpper.Max, ReflectFields->UpperBgColor);

    const float minFrame = _system->GetMinFrame(); // minimum frame in the timeline
    const float maxFrame = _system->GetMaxFrame(); // maximum frame in the timeline
    if (maxFrame < minFrame)
    {
        drawList->AddText(_canvasRectLower.Min, ImColor(1.0f, 1.0f, 1.0f, 1.0f), "Invalid Min-Max Frame");
        drawList->AddRectFilled(_canvasRectLower.Min, _canvasRectLower.Max, ReflectFields->InvalidColor);
        return false;
    }
    else
    {
        drawList->AddRectFilled(_canvasRectLower.Min, _canvasRectLower.Max, ReflectFields->LowerInvaildBgColor);
    }
    return true;
}

void EditorSequencer::End() 
{
    ImGui::PopClipRect();
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
    //ImU32  Color;
    //float  Thickness;
};

void EditorSequencer::DrawCanvas()
{
    auto& io = ImGui::GetIO();
    auto* drawList = ImGui::GetWindowDrawList();
   
    const bool   isContain  = _canvasRect.Contains(io.MousePos);

    const ImVec2 mousePos   = io.MousePos - _canvasRect.Min; // mouse position relative to the canvas
    const ImVec2 canvasSize = _canvasRect.GetSize();         // size of the canvas

    const float curFrame    = _system->GetCurrentFrame();   // current frame in the timeline
    const float minFrame    = _system->GetMinFrame();       // minimum frame in the timeline
    const float maxFrame    = _system->GetMaxFrame();       // maximum frame in the timeline

    const int   lineUnit    = GetLineUnit();
    const float unitDistane = ReflectFields->UnitSize * ReflectFields->ViewScale;
    const float offsetX     = _viewPosition.x * ReflectFields->ViewScale;
    const float startX      = fmodf(offsetX, unitDistane);

    bool isSnapped = false;
    ImVec2 snapPos = ImVec2(0.0f, 0.0f);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Draw Background
    //////////////////////////////////////////////////////////////////////////////////////////////////
    ImVec2 validRectMin = ImVec2(offsetX + (minFrame * unitDistane), 0.0f) + _canvasRectLower.Min;
    ImVec2 validRectMax = ImVec2(offsetX + (maxFrame * unitDistane), canvasSize.y) + _canvasRectLower.Min;
    drawList->AddRectFilled(validRectMin, validRectMax, ReflectFields->LowerVaildBgColor);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Draw Line
    //////////////////////////////////////////////////////////////////////////////////////////////////
    std::vector<InteractionData> Interactions;
    Interactions.reserve(20);

    int unitFrame = GetFrameToInt(-_viewPosition.x, ReflectFields->UnitSize);

    // Draw Min, Max Lines
    {
        ImVec2 canvasHeight = ImVec2(0.0f, _canvasRectLower.GetHeight());
        ImVec2 minStart     = ImVec2(validRectMin.x, _canvasRectLower.Min.y);
        ImVec2 minEnd       = minStart + canvasHeight;
        ImVec2 maxStart     = ImVec2(validRectMax.x, _canvasRectLower.Min.y);
        ImVec2 maxEnd       = maxStart + canvasHeight;
        drawList->AddLine(minStart, minEnd, ReflectFields->ThinLineColor, 1.0f);
        drawList->AddLine(maxStart, maxEnd, ReflectFields->ThinLineColor, 1.0f);
        Interactions.emplace_back(minStart, minEnd);
        Interactions.emplace_back(maxStart, maxEnd);
    }

    // Draw Unit Lines
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
        drawList->AddLine(start, middle, ReflectFields->ThickLineColor, 2.0f);
        drawList->AddLine(middle, end, ReflectFields->ThinLineColor, 1.0f);
        Interactions.emplace_back(middle, end);
    }
    // Draw Current Frame Line
    {
        ImVec2 linePos     = ImVec2(curFrame * ReflectFields->UnitSize, 0.0f);
        ImVec2 canvasSapce = PositionToCanvasSapce(linePos);

        ImVec2 start = canvasSapce + _canvasRectLower.Min;
        ImVec2 end   = start + ImVec2(0.0f, canvasSize.y);

        drawList->AddLine(start, end, ReflectFields->CurFrameLineColor, 2.0f);
        if (false == _isDraggingStampBar)
        {
            Interactions.emplace_back(start, end);
        }
        { // Draw Point Rect
            float  tipDepth = 0.3f;
            ImVec2 size(_canvasUpperHeight * 0.5f, _canvasUpperHeight);
            ImVec2 pos(start.x - (size.x * 0.5f), start.y - size.y);
            ImRect rect(pos, pos + size - ImVec2(0.f, 1.0f - tipDepth));
            ImRect pointRect(pos, pos + size);

            bool isHovered   = pointRect.Contains(io.MousePos);
            bool isMouseDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);
            _isDraggingStampBar = (true == isHovered) && (true == isMouseDown);

            if (true == _isDraggingStampBar)
            {
                float frame = ImClamp(_indicateFrame, minFrame, maxFrame);
                _system->SetCurrentFrame(frame);
            }

            // TopLeft
            drawList->PathLineTo(rect.GetTL());
            // TopRight
            drawList->PathLineTo(rect.GetTR());
            // BottomRight
            drawList->PathLineTo(rect.GetBR());
            // Tip Center
            drawList->PathLineTo(start);
            // BottomLeft
            drawList->PathLineTo(rect.GetBL());

            // 최종 그리기
            ImU32 color = _isDraggingStampBar ? ReflectFields->CurFrameLineColor + 20 : ReflectFields->CurFrameLineColor;
            drawList->PathFillConvex(color);
        }
    }

    // Draw Notify
    const auto& notifyQueue = _system->GetTimelineNotifyList();
    for (const auto& notify : notifyQueue)
    {
        if (nullptr == notify)
        {
            continue;
        }

        ImVec2 point = ImVec2(offsetX + (notify->Time * unitDistane), _canvasRectLower.GetHeight() * 0.5f) + _canvasRectLower.Min;
        float lenght = 5.0f;

        drawList->PathLineTo(point + ImVec2(0.0f, lenght));
        drawList->PathLineTo(point + ImVec2(-lenght, 0.0f));
        drawList->PathLineTo(point + ImVec2(0.0f, -lenght));
        drawList->PathLineTo(point + ImVec2(lenght, 0.0f));

        bool isValid = notify->Time >= minFrame && notify->Time <= maxFrame;

        drawList->PathFillConvex(isValid ? ReflectFields->NotifyColor : ReflectFields->InvalidColor);
        Interactions.emplace_back(point, point);
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
        float linePosX     = isSnapped ? snapPos.x : mousePos.x;
        float canvasSapceX = -_viewPosition.x + linePosX / ReflectFields->ViewScale;
        if (false == _isOpenedPopup)
        {
            _indicateFrame = GetFrameToFloat(canvasSapceX, ReflectFields->UnitSize);
        }
       
        //ImVec2 start = ImVec2(linePosX, _canvasUpperHeight) + _canvasRectUpper.Min;
        //ImVec2 end   = ImVec2(linePosX, canvasSize.y) + _canvasRectLower.Min;
        ImVec2 start = ImVec2(offsetX + (_indicateFrame * unitDistane), 0.0f) + _canvasRectLower.Min;
        ImVec2 end   = ImVec2(offsetX + (_indicateFrame * unitDistane), _canvasRectLower.GetHeight()) + _canvasRectLower.Min;

        std::string frameText = std::format("{:.3f}", _indicateFrame);
        if (true == _system->IsVaildFrame(_indicateFrame))
        {
            drawList->AddText(start + ImVec2(5.0f, 0), ImColor(1.0f, 1.0f, 1.0f, 1.0f), frameText.c_str());
            drawList->AddLine(start, end, ReflectFields->FollowLineColor, 2.0f);
        }
        else
        {
            drawList->AddText(start + ImVec2(5.0f, 0), ReflectFields->InvalidColor, frameText.c_str());
            drawList->AddLine(start, end, ReflectFields->InvalidColor, 4.0f);
        }
    }
}

bool EditorSequencer::WheelZooming() 
{
    ImGuiIO& io = ImGui::GetIO();

    bool isZooming = IsWheelZooming();

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
        _ZoomPosition = io.MousePos - _frameRect.Min;
    }

    float& viewScale = ReflectFields->ViewScale;
    if (false == std::isnan(_ZoomPosition.x))
    {
        ImVec2 mouseWPosPre, mouseWPosPost;
        mouseWPosPre.x  = _ZoomPosition.x / viewScale;
        viewScale       = ImLerp(ReflectFields->ViewScale, _viewLerpTarget, ReflectFields->ViewLerpScale);
        mouseWPosPost.x = _ZoomPosition.x / viewScale;
        _viewPosition += mouseWPosPost - mouseWPosPre;
    }

    return isZooming;
}

bool EditorSequencer::Dragging()
{
    ImGuiIO& io = ImGui::GetIO();
    if (true == IsDragging(ImGuiMouseButton_Right))
    {
        ImVec2 mouseDelta = io.MouseDelta;
        bool isMoved = (mouseDelta.x != 0.0f);
        if (true == isMoved)
        {
            _viewPosition += ImVec2(mouseDelta.x / ReflectFields->ViewScale, 0.0f);
            _isDraggingCanvas = true;
        }
    }
    else
    {
        _isDraggingCanvas = false;
    }
    return _isDraggingCanvas;
}

bool EditorSequencer::ContextMenu()
{
    ImGuiIO& io = ImGui::GetIO();
    bool isMouseRBReleased = ImGui::IsMouseReleased(ImGuiMouseButton_Right);
    bool isMouseLBReleased = ImGui::IsMouseReleased(ImGuiMouseButton_Left);
    bool isContain      = _canvasRect.Contains(io.MousePos);
    bool isUpperContain = _canvasRectUpper.Contains(io.MousePos);
    bool isLowerContain = _canvasRectLower.Contains(io.MousePos);

    if (true == isMouseRBReleased && true == isLowerContain && false == _isDraggingCanvas)
    {
        ImGui::OpenPopup("LowerContextMenu");
        _isOpenedPopup = true;
    }

    if (ImGui::BeginPopupContextItem("LowerContextMenu"))
    {
        if (ImGui::BeginMenu("Add Notify"))
        {
            auto& table = _system->GetInstanceConstructors();
            for (const auto& [key, func] : table)
            {
                if (ImGui::MenuItem(key.c_str() + 6))
                {
                    _system->AddNotify(_indicateFrame, key);
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }
    else
    {
        _isOpenedPopup = false;
    }
    return _isOpenedPopup;
}

bool EditorSequencer::IsWheelZooming() const
{
    ImGuiIO& io       = ImGui::GetIO();
    bool isContain    = _canvasRect.Contains(io.MousePos);
    bool isMouseValid = ImGui::IsMousePosValid();
    bool isWheelMoved = (io.MouseWheel < -FLT_EPSILON || io.MouseWheel > FLT_EPSILON);
    return isContain && isMouseValid && isWheelMoved;
}

bool EditorSequencer::IsDragging(int button) const
{
    ImGuiIO& io = ImGui::GetIO();
    bool isContain      = _canvasRect.Contains(io.MousePos);
    bool isMouseClicked = ImGui::IsMouseDown(button);
    return isContain && isMouseClicked;
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

int EditorSequencer::GetFrameToInt(float x, float unitSize) const
{
    return static_cast<int>(GetFrameToFloat(x, unitSize));
}

float EditorSequencer::GetFrameToFloat(float x, float unitSize) const
{
    return x / unitSize;
}

void EditorSequencer::SetViewPositionXFromFrame(float frame) 
{
    _viewPosition.x = ReflectFields->UnitSize * frame;
}

ImVec2 EditorSequencer::PositionToCanvasSapce(const ImVec2& pos) const
{
    return ImVec2(_viewPosition + pos) * ReflectFields->ViewScale;
}
