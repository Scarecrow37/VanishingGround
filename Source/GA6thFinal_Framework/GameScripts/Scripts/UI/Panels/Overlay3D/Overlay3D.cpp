#include "pchScripts.h"
#include "Overlay3D.h"

#include "Scripts/Camera/CameraComponent.h"

UMREAL_COMPONENT(Overlay3DPanel)

Overlay3DPanel::Overlay3DPanel() = default;

void Overlay3DPanel::Awake()
{
    OverlayPanel::Awake();

    FindComponents();
}

void Overlay3DPanel::Start()
{
    OverlayPanel::Start();

    if (const auto sharedTargetObject = _targetObject.lock())
    {
        transform->Position = sharedTargetObject->transform->GetWorldPosition();
    }

    // 초기 월드 좌표는 ViewportToWorld를 사용하여 올바르게 변환
    if (const auto sharedCameraComponent = _targetCameraComponent.lock())
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

void Overlay3DPanel::FindComponents()
{
    const std::string& targetCameraTag = ReflectFields->TargetCameraTag;
    _targetCameraComponent             = GameObject::FindComponentWithTag<CameraComponent>(targetCameraTag.data());

    _targetObject = GameObject::FindWithTag(ReflectFields->TargetObjectTag);
}

void Overlay3DPanel::UpdateCameraViewMatrix()
{
    if (const auto sharedCameraComponent = _targetCameraComponent.lock())
    {
        const Vector3 viewportPos = sharedCameraComponent->WorldToViewport(transform->Position);
        const POINT   newPoint    = POINT{.x = static_cast<LONG>(viewportPos.x) + _offsetFromTarget.x,
                                          .y = static_cast<LONG>(viewportPos.y) + _offsetFromTarget.y};
        const MARGIN  margin      = Margin;
        Point                     = newPoint - (Offset + margin.LeftTop());
    }
}
