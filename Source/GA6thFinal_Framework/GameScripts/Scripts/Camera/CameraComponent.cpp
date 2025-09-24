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

Vector3 CameraComponent::WorldToNDC(const Vector3& worldPos)
{
    if (_camera)
    {
        const Matrix& proj = _camera->GetProjectionMatrix();
        const Matrix& view = _camera->GetViewMatrix();
        return WorldToNDC(worldPos, view, proj);
    }
    return Vector3::Zero;
}

Vector3 CameraComponent::WorldToNDC(const Vector3& worldPos, const Matrix& viewMatrix, const Matrix& projMatrix)
{
    Matrix vp = viewMatrix * projMatrix;

    // 월드좌표 >> 클립공간
    Vector4 wolrd   = Vector4(worldPos.x, worldPos.y, worldPos.z, 1.0f);
    Vector4 clipPos = Vector4::Transform(wolrd, vp);

    // NDC로 변환
    Vector3 ndc;
    ndc.x = clipPos.x / clipPos.w;
    ndc.y = clipPos.y / clipPos.w;
    ndc.z = clipPos.z / clipPos.w;

    return ndc;
}

Vector3 CameraComponent::WorldToViewport(const Vector3& worldPos)
{
    if (_camera)
    {
        const Matrix& proj = _camera->GetProjectionMatrix();
        const Matrix& view = _camera->GetViewMatrix();
        return WorldToViewport(worldPos, view, proj);
    }
    return Vector3::Zero;
}

Vector3 CameraComponent::WorldToViewport(const Vector3& worldPos, const Matrix& viewMatrix, const Matrix& projMatrix)
{
    const SIZE&   resolution = UmGraphics.GetResolution();
    const Vector3 ndc        = WorldToNDC(worldPos, viewMatrix, projMatrix);

    Vector3 screen;
    screen.x = (ndc.x + 1.0f) * 0.5f * resolution.cx;
    screen.y = (1.0f - ndc.y) * 0.5f * resolution.cy;
    screen.z = ndc.z;

    return screen;
}

Vector3 CameraComponent::ViewportToWorld(const Vector3& screenPos)
{
    if (_camera)
    {
        const float camNear     = _camera->GetNearZ();
        const float camFar      = _camera->GetFarZ();
        const SIZE& resolution  = UmGraphics.GetResolution();
        if (resolution.cx <= 0 || resolution.cy <= 0)
        {
            return Vector3::Zero;
        }

        Vector3 ndc;
        ndc.x = (screenPos.x / (float)resolution.cx) * 2.0f - 1.0f;
        ndc.y = 1.0f - (screenPos.y / (float)resolution.cy) * 2.0f;
        ndc.z = screenPos.z * 2.0f - 1.0f;                                       

        const Matrix& invProj   = _camera->GetProjectionInverseMatrix();
        const Matrix  invView   = _camera->GetViewMatrix().Invert();
        const Vector4 clip      = Vector4(ndc.x, ndc.y, ndc.z, 1.0f);
        Vector4 viewPos = Vector4::Transform(clip, invProj);
        if (viewPos.w != 0.0f)
        {
            viewPos /= viewPos.w;
        }
        Vector4 worldPos = Vector4::Transform(viewPos, invView);
        if (worldPos.w != 0.0f)
        {
            worldPos /= worldPos.w;
        }

        return Vector3(worldPos.x, worldPos.y, worldPos.z);
    }
    return Vector3::Zero;
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

