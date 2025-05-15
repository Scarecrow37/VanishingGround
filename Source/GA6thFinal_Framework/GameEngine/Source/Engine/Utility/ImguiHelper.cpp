#include "pch.h"
#include "Editor/DynamicCamera/EditorDynamicCamera.h"

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

bool ImGuiHelper::HoveredToolTip(std::string_view toolTip)
{
    bool isHovered = ImGui::IsItemHovered();
    if (isHovered)
    {
        ImGui::SetTooltip(toolTip.data());
    }
    return isHovered;
}

namespace ImGuiHelper
{
    //내부용
    static bool DrawManipulate(
      EditorDynamicCamera* pDynamicCamera, 
      Camera* pCamera, 
      Matrix* pObjectMatrix,
      DrawManipulateDesc & desc);
}

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
            viewManipulateMatrix.Decompose(scale, rotation, position);
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
    return manipulateResult;
}

