#include "pchScripts.h"
#include "CameraComponent.h"

CameraComponent::CameraComponent() 
    : 
    Component(TYPE::CAMERA),
    _isDirty(false) 
{

}
CameraComponent::~CameraComponent()
{
    if (true == ReflectFields->IsMainCam)
    {
        ResetMainCamera();
    }
}

void CameraComponent::ImGuiDrawPropertysEvent() 
{
    if (ImGui::Button("Set As Main"))
    {
        SetMainCamera();
    }
    ImGuiHelper::HoveredToolTip((const char*)u8"메인 카메라로 설정합니다.");
}

void CameraComponent::DeserializedReflectEvent() 
{
    _isDirty = true;
}

void CameraComponent::OnDrawDebug() 
{

}

void CameraComponent::OnDrawDebugSelected() 
{
    BoundingFrustum frustum;
    BoundingFrustum::CreateFromMatrix(frustum, _camera->GetProjectionMatrix());
    frustum.Transform(frustum, _camera->GetWorldMatrix());
    UmGraphics.DebugDraw("Editor", frustum, DEBUG_COLOR);
}

