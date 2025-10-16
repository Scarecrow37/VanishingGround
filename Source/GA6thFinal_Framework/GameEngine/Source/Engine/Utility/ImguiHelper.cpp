#include "pch.h"
#include "Editor/DynamicCamera/EditorDynamicCamera.h"
#include "ImguiHelper.h"

std::array<float, 4> ImGuiHelper::ImVec4ToArray(const ImVec4& vec4)
{
    std::array<float, 4> array{};
    std::memcpy(array.data(), &vec4.x, sizeof(array));
    return array;
}

ImVec4 ImGuiHelper::ArrayToImVec4(const std::array<float, 4>& array)
{
    ImVec4 vec4;
    std::memcpy(&vec4.x, array.data(), sizeof(array));
    return vec4;
}

bool ImGuiHelper::HoveredToolTip(std::string_view toolTip, int flags)
{
    bool isHovered = ImGui::IsItemHovered(flags);
    if (isHovered)
    {
        ImGui::SetTooltip(toolTip.data());
    }
    return isHovered;
}

bool ImGuiHelper::HoveredToolTip(std::u8string_view toolTip, int flags)
{
    return HoveredToolTip((const char*)toolTip.data(), flags);
}

namespace ImGuiHelper
{
    //내부용
    static bool DrawManipulate(
      EditorDynamicCamera* pDynamicCamera, 
      Camera* pCamera, 
      Matrix* pObjectMatrix,
      DrawManipulateDesc & desc);

    bool BeginComboInput(const char* label, const char* preview_value, ImGuiInputTextFlags inputTextFlags, ImGuiComboFlags comboFlags)
    {
        const ImGuiStyle& style  = ImGui::GetStyle();
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        const ImGuiID id = window->GetID(label);

        ImGui::PushID(id);

        const char*   popupLabel    = "##ComboPopup";
        const char*   buttonLabel   = "##DownArrow";
        const ImGuiID popupID       = ImGui::GetID(popupLabel);
        const float   arrowSize     = (comboFlags & ImGuiComboFlags_NoArrowButton) ? 0.0f : ImGui::GetFrameHeight();
        const ImVec2  labelSize     = ImGui::CalcTextSize(preview_value, NULL, true);
        const float   previewWidth  = ((comboFlags & ImGuiComboFlags_WidthFitPreview) && (preview_value != NULL)) ? ImGui::CalcTextSize(preview_value, NULL, true).x : 0.0f;
        const float   w             = (comboFlags & ImGuiComboFlags_NoPreview) ? arrowSize : ((comboFlags & ImGuiComboFlags_WidthFitPreview) ? (arrowSize + previewWidth + style.FramePadding.x * 2.0f) : ImGui::CalcItemWidth());
        const ImRect  bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, labelSize.y + style.FramePadding.y * 2.0f));
        const ImVec2  cursor        = ImGui::GetCursorPos();

        char*  buffer     = (char*)preview_value;
        size_t bufferSize = strlen(preview_value);
        ImGui::SetNextItemWidth(w);
        ImGui::InputText(label, buffer, bufferSize, inputTextFlags);
        ImGui::SameLine();
        ImGui::SetCursorPosX(cursor.x + w);
        bool isPopupOpen = ImGui::IsPopupOpen(popupLabel, ImGuiPopupFlags_None);
        bool isPressed   = ImGui::ArrowButton(buttonLabel, ImGuiDir_Down);
        if (true == isPressed && false == isPopupOpen)
        {
            ImGui::OpenPopup(popupLabel, ImGuiPopupFlags_None);
            isPopupOpen = true;
        }
        ImGui::PopID();
        return ImGui::BeginComboPopup(popupID, bb, comboFlags);
    }
    bool BeginComboInput(const char* label, std::string* inputBuffer, ImGuiInputTextFlags inputTextFlags, ImGuiComboFlags comboFlags)
    {
        const ImGuiStyle& style  = ImGui::GetStyle();
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        const ImGuiID id = window->GetID(label);

        ImGui::PushID(id);

        const char*   popupLabel    = "##ComboPopup";
        const char*   buttonLabel   = "##DownArrow";
        const ImGuiID popupID       = ImGui::GetID(popupLabel);
        const float   arrowSize     = (comboFlags & ImGuiComboFlags_NoArrowButton) ? 0.0f : ImGui::GetFrameHeight();
        const ImVec2  labelSize     = ImGui::CalcTextSize(inputBuffer->c_str(), NULL, true);
        const float   previewWidth  = ((comboFlags & ImGuiComboFlags_WidthFitPreview) && (inputBuffer != NULL)) ? ImGui::CalcTextSize(inputBuffer->c_str(), NULL, true).x : 0.0f;
        const float   w             = (comboFlags & ImGuiComboFlags_NoPreview) ? arrowSize : ((comboFlags & ImGuiComboFlags_WidthFitPreview) ? (arrowSize + previewWidth + style.FramePadding.x * 2.0f) : ImGui::CalcItemWidth());
        const ImRect  bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, labelSize.y + style.FramePadding.y * 2.0f));
        const ImVec2  cursor        = ImGui::GetCursorPos();

        ImGui::SetNextItemWidth(w);
        ImGui::InputText(label, inputBuffer, inputTextFlags);
        ImGui::SameLine();
        ImGui::SetCursorPosX(cursor.x + w);
        bool isPopupOpen = ImGui::IsPopupOpen(popupLabel, ImGuiPopupFlags_None);
        bool isPressed   = ImGui::ArrowButton(buttonLabel, ImGuiDir_Down);
        if (true == isPressed && false == isPopupOpen)
        {
            ImGui::OpenPopup(popupLabel, ImGuiPopupFlags_None);
            isPopupOpen = true;
        }
        ImGui::PopID();
        return ImGui::BeginComboPopup(popupID, bb, comboFlags);
    }
    ImRect GetWindowTabBarRect()
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        float titleBarHeight = window->TitleBarHeight;
        ImRect tabRect = ImRect(window->Pos, window->Pos + ImVec2(window->Size.x, titleBarHeight));
        return tabRect;
    }
    void Separator(float upPadding, float downPadding)
    {
        ImGui::Dummy(ImVec2(0.0f, upPadding));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, downPadding));
    }
    void Separator(float spacing) 
    {
        ImGui::Dummy(ImVec2(0.0f, spacing));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, spacing));
    }
    void TextWithVerticalSeparator(const char* text, float startX)
    {
        TextWithVerticalSeparatorEx(text, startX);
        float availX = ImGui::GetContentRegionAvail().x;
        ImGui::SetNextItemWidth(availX);
    }
    void TextWithVerticalSeparatorEx(const char* text, float startX) 
    {
        ImGui::Text(text);
        if (FLT_MAX == startX)
        {
            startX = ImGui::GetCursorPosX();
            startX += ImGui::CalcTextSize(text).x;
            startX += ImGui::GetStyle().ItemSpacing.x;
        }
        ImGui::SameLine(startX);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
        ImGui::SameLine();
    }
    void CenterText(const char* text)
    {
        if (text == nullptr)
            return;

        float columnWidth = ImGui::GetColumnWidth();
        float textWidth   = ImGui::CalcTextSize(text).x;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (columnWidth - textWidth) * 0.5f);
        ImGui::TextUnformatted(text);
    }
} // namespace ImGuiHelper

bool ImGuiHelper::DrawManipulate(
    Camera* pCamera, 
    Matrix* pObjectMatrix, 
    DrawManipulateDesc& desc)
{
    return DrawManipulate(nullptr, pCamera, pObjectMatrix, desc);
}

bool ImGuiHelper::DrawManipulate(
    EditorDynamicCamera* pDynamicCamera, 
    Matrix* pObjectMatrix, 
    DrawManipulateDesc& desc)
{
    return DrawManipulate(pDynamicCamera, nullptr, pObjectMatrix, desc);
}

static bool ImGuiHelper::DrawManipulate(
    EditorDynamicCamera* pDynamicCamera,
    Camera* pCamera,
    Matrix* pObjectMatrix,
    DrawManipulateDesc& desc)
{
    Camera* realCamera = nullptr;
    if (nullptr != pDynamicCamera)
    {
        realCamera = pDynamicCamera->GetCamera().get();  
    }
    else if (nullptr != pCamera)
    {
        realCamera = pCamera;
    }   
    if (nullptr == realCamera)
    {
        return false;
    }
    Camera& camera = *realCamera;

    const Matrix& view = camera.GetViewMatrix();
    const Matrix& projection = camera.GetProjectionMatrix();
    Matrix& objectMatrix = *pObjectMatrix;
   
    float* pSnap = desc.UseSnap ? desc.Snap.data() : nullptr;
    bool manipulateResult = ImGuizmo::Manipulate(
        (float*)view.m,
        (float*)projection.m,
        desc.Operation,
        desc.Mode,
        (float*)objectMatrix.m, 
        (float*)nullptr,
        pSnap);

    /*
    auto& viewDesc = desc.ViewDesc;
    if (0.f < viewDesc.Size.x && 0.f < viewDesc.Size.y)
    {
        Vector3 camPos = Vector3(&camera.GetWorldMatrix()._41);
        Vector3 objPos = Vector3(&objectMatrix._41);
        float length = Vector3::Distance(camPos, objPos);

        ImVec2 viewManipulatePosition;
        viewManipulatePosition.x = viewDesc.ClientRight - viewDesc.Size.x;
        viewManipulatePosition.y = viewDesc.ClientTop;

        constexpr Matrix inversionMatrix(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1);
        Matrix viewManipulateMatrix = (inversionMatrix * view * inversionMatrix);
        ImGuizmo::ViewManipulate((float*)viewManipulateMatrix.m, length, viewManipulatePosition, viewDesc.Size, viewDesc.BackgroundColor);

        ImVec2 mousePos = ImGui::GetIO().MousePos;
        ImVec2 rectMin  = viewManipulatePosition;
        ImVec2 rectMax = ImVec2(viewManipulatePosition.x + viewDesc.Size.x, viewManipulatePosition.y + viewDesc.Size.y);
        bool isMouseHoveringRect = (mousePos.x >= rectMin.x && mousePos.x <= rectMax.x && mousePos.y >= rectMin.y && mousePos.y <= rectMax.y);
        if (true == isMouseHoveringRect)
        {
            viewManipulateMatrix = inversionMatrix * viewManipulateMatrix * inversionMatrix;
            viewManipulateMatrix = viewManipulateMatrix.Invert();
            Vector3    position;
            Quaternion rotation;
            Vector3    scale;
            bool decomposeResult = viewManipulateMatrix.Decompose(scale, rotation, position);
            if(decomposeResult)
            {
                if (nullptr != pDynamicCamera)
                {
                    pDynamicCamera->SetPosition(position);
                    pDynamicCamera->SetRotation(rotation);
                }
                else if (nullptr != pCamera)
                {
                    pCamera->SetPosition(position);
                    pCamera->SetRotation(rotation);
                }   
            }        
        }
    }
    */
    return manipulateResult;
}

