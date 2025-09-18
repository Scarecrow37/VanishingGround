#include "pchScripts.h"
#include "CameraComponent.h"


UMREAL_COMPONENT(CameraComponent)

CameraComponent::CameraComponent() 
    : 
    Component(TYPE::CAMERA),
    _isDirty(false) 
{
   

}
CameraComponent::~CameraComponent()
{
    if (ESceneManager::Engine::GetMainCamera() == this)
    {
        ResetMainCamera();
    }
}

void CameraComponent::Reset() 
{
    Base::Reset();
#ifdef _UMEDITOR
    _gizmo.SetIconTexture(SceneGizmo::DefaultIcon::CAMERA);
#endif
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
    if (gameObject->IsValid())
    {
        if (true == ReflectFields->IsMainCam)
        {
            UmGraphics.SetCamera("Game", _camera);
            ESceneManager::Engine::SetSceneMainCamera(this);
        }
    }
    _isDirty = true;
}

void CameraComponent::OnDrawDebug() 
{
#ifdef _UMEDITOR
    _gizmo.DrawIcon();
#endif
}

void CameraComponent::OnDrawDebugSelected() 
{
    BoundingFrustum frustum;
    BoundingFrustum::CreateFromMatrix(frustum, _camera->GetProjectionMatrix());
    frustum.Transform(frustum, _camera->GetWorldMatrix());
    UmGraphics.DebugDraw3D("Editor", frustum, DEBUG_COLOR);
}

