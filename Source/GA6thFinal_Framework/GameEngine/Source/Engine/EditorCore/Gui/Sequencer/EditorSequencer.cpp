#include "pch.h"
#include "EditorSequencer.h"

EditorSequencer::EditorSequencer() 
    : _system(nullptr)
    , _canvasRect(ImVec2(0, 0)
    , ImVec2(0, 0))
    , _viewLerpTarget(1.0f)
    , _zoomMin(0.05f)
    , _zoomMax(1.0f)
    , _viewPosition(ImVec2(0, 0))
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

    UpdateCanvas();

    BeginCanvas();
    {
        DrawCanvas();
    }
    EndCanvas();

    ImGui::PopID();
}

void EditorSequencer::UpdateCanvas()
{
    const ImVec2 windowPos  = ImGui::GetCursorScreenPos();
    const ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    _canvasRect = ImRect(windowPos, windowPos + canvasSize);

    ImGuiIO& io = ImGui::GetIO();
    
    bool isContain = _canvasRect.Contains(io.MousePos);
    bool isCtrl    = ImGui::IsKeyPressed(ImGuiKey_LeftCtrl, false);
    if (true == isContain && false == isCtrl)
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
    }
    ImVec2 mouseWPosPre, mouseWPosPost;
    
    mouseWPosPre.x = (io.MousePos.x - ImGui::GetCursorScreenPos().x) / ReflectFields->ViewScale;
    ReflectFields->ViewScale = ImLerp(ReflectFields->ViewScale, _viewLerpTarget, ReflectFields->ViewLerpScale);
    mouseWPosPost.x = (io.MousePos.x - ImGui::GetCursorScreenPos().x) / ReflectFields->ViewScale;
    if (ImGui::IsMousePosValid())
    {
        _viewPosition += mouseWPosPost - mouseWPosPre;
    }
}

void EditorSequencer::BeginCanvas() 
{
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImGui::PushClipRect(_canvasRect.Min, _canvasRect.Max, true);

    _upperFrameY     = 20.0f;
    _canvasRectUpper = ImRect(_canvasRect.Min, ImVec2(_canvasRect.Max.x, _canvasRect.Min.y + _upperFrameY));
    _canvasRectLower = ImRect(ImVec2(_canvasRect.Min.x, _canvasRect.Min.y + _upperFrameY), _canvasRect.Max);
    drawList->AddRectFilled(_canvasRectUpper.Min, _canvasRectUpper.Max, ReflectFields->UpperBgColor);
    drawList->AddRectFilled(_canvasRectLower.Min, _canvasRectLower.Max, ReflectFields->LowerBgColor);
}

void EditorSequencer::EndCanvas() 
{
    ImGui::PopClipRect();
}

void EditorSequencer::DrawCanvas()
{
    ImDrawList*  drawList = ImGui::GetWindowDrawList();
    const ImVec2 windowPos = ImGui::GetCursorScreenPos();
    const ImVec2 canvasSize = ImGui::GetContentRegionAvail();

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Draw Line
    //////////////////////////////////////////////////////////////////////////////////////////////////
    int   lineUnit  = GetLineUnit();
    float lineSpace = ReflectFields->UnitSize * ReflectFields->ViewScale;
    float offsetX   = _viewPosition.x * ReflectFields->ViewScale;
    float startX    = fmodf(offsetX, lineSpace);
    int   indexX    = GetLineIndexInt(-_viewPosition.x, ReflectFields->UnitSize);
    for (float x = startX; x < canvasSize.x; x += lineSpace, ++indexX)
    {
        if (indexX % lineUnit != 0)
        {
            continue;
        }
        ImVec2 start  = ImVec2(x, 0.0f) + _canvasRectUpper.Min;
        ImVec2 middle = ImVec2(x, 0.0f) + _canvasRectLower.Min;
        ImVec2 end   = ImVec2(x, canvasSize.y) + _canvasRectLower.Min;
        std::string frame = std::to_string(indexX);
        drawList->AddText(start + ImVec2(5.0f, 0), ImColor(1.0f, 1.0f, 1.0f, 1.0f), frame.c_str());
        drawList->AddLine(start, middle, ReflectFields->ThickLineColor, 3.0f);
        drawList->AddLine(middle, end, ReflectFields->ThinLineColor, 1.0f);
    }
    // Draw FollowLine
    auto& io = ImGui::GetIO();
    if (_canvasRectLower.Contains(io.MousePos))
    {
        ImVec2 mousePos = io.MousePos - _canvasRectLower.Min;
        float indexX = GetLineIndexFloat(-_viewPosition.x + mousePos.x, ReflectFields->UnitSize);

        float alignedX = startX + (indexX - GetLineIndexInt(-_viewPosition.x, ReflectFields->UnitSize)) * lineSpace;


        ImVec2 start = ImVec2(mousePos.x, _upperFrameY) + _canvasRectUpper.Min;
        ImVec2 end   = ImVec2(mousePos.x, canvasSize.y) + _canvasRectLower.Min;

        drawList->AddLine(start, end, ReflectFields->FollowLineColor, 3.0f);

        // 소수점 3자리 표시
        std::string frameText = std::format("{:.3f}", indexX);
        drawList->AddText(start + ImVec2(5.0f, 0), ImColor(1.0f, 1.0f, 1.0f, 1.0f), frameText.c_str());
    }
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

int EditorSequencer::GetLineIndexInt(float x, float unitSize) const
{
    return static_cast<int>((x - _canvasRectLower.Min.x) / unitSize);
}

float EditorSequencer::GetLineIndexFloat(float x, float unitSize) const
{
    return (x - _canvasRectLower.Min.x) / unitSize;
}
