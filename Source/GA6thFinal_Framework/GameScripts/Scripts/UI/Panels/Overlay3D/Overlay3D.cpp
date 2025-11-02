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

    _originPoint = Point;
    const MARGIN margin     = Margin;
    const POINT  leftTopMargin = margin.LeftTop();
    _offsetAndMarginLT         = Offset + leftTopMargin;
    
    // 초기 월드 좌표는 ViewportToWorld를 사용하여 올바르게 변환
    if (auto sharedCameraComponent = _targetCameraComponent.lock())
    {
        const POINT absolutePos = AbsolutePosition;
        const Vector3 screenPos = Vector3(static_cast<float>(absolutePos.x), static_cast<float>(absolutePos.y), 0);
        _targetWorldPosition = sharedCameraComponent->ViewportToWorld(screenPos);
        _targetWorldPosition.z    = Z;
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
    _targetCameraComponent       = GameObject::FindComponentWithTag<CameraComponent>(targetCameraTag.data());
}

void Overlay3DPanel::UpdateCameraViewMatrix()
{
    if (const auto sharedCameraComponent = _targetCameraComponent.lock())
    {
        // 월드 좌표를 현재 카메라의 ViewProjection으로 스크린 좌표로 변환
        const Vector3 screenPos = sharedCameraComponent->WorldToViewport(_targetWorldPosition);
        const POINT newPoint = POINT{
            .x = static_cast<LONG>(screenPos.x) - _offsetAndMarginLT.x, 
            .y = static_cast<LONG>(screenPos.y) - _offsetAndMarginLT.y
        };
        Point = newPoint;
    }
}
