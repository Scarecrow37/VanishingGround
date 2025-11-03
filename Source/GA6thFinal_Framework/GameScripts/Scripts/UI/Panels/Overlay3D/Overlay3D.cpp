#include "pchScripts.h"
#include "Overlay3D.h"

#include "Scripts/Camera/CameraComponent.h"

UMREAL_COMPONENT(Overlay3DPanel)

Overlay3DPanel::Overlay3DPanel() = default;

void Overlay3DPanel::SetPosition(const Vector3& position)
{
    transform->Position = position;

    // 초기 월드 좌표는 ViewportToWorld를 사용하여 올바르게 변환
    const std::string&                   targetCameraTag = ReflectFields->TargetCameraTag;
    const std::weak_ptr<CameraComponent> targetCamera =
        GameObject::FindComponentWithTag<CameraComponent>(targetCameraTag.data());
    if (const auto sharedCameraComponent = targetCamera.lock())
    {
        const Vector3 viewportPos      = sharedCameraComponent->WorldToViewport(transform->Position);
        const POINT   absolutePosition = AbsolutePosition;
        const POINT   viewportPositionPoint =
            POINT{.x = static_cast<LONG>(viewportPos.x), .y = static_cast<LONG>(viewportPos.y)};
        _offsetFromTarget = absolutePosition - viewportPositionPoint;
    }
}

void Overlay3DPanel::Update()
{
    OverlayPanel::Update();

    UpdateCameraViewMatrix();
}

void Overlay3DPanel::UpdateCameraViewMatrix()
{
    if (CameraComponent* mainCamera = CameraComponent::MainCamera())
    {
        const Vector3 viewportPos = mainCamera->WorldToViewport(transform->Position);
        const POINT   newPoint    = POINT{.x = static_cast<LONG>(viewportPos.x) + _offsetFromTarget.x,
                                          .y = static_cast<LONG>(viewportPos.y) + _offsetFromTarget.y};
        const MARGIN  margin      = Margin;
        Point                     = newPoint - (Offset + margin.LeftTop());
    }
}
