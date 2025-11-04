#include "pchScripts.h"
#include "Overlay3D.h"

#include "Scripts/Camera/CameraComponent.h"

UMREAL_COMPONENT(Overlay3DPanel)

Overlay3DPanel::Overlay3DPanel() = default;

void Overlay3DPanel::SetPosition(const Vector3& position)
{
    transform->Position = position;
    //const std::weak_ptr<CameraComponent> editorCamera =
    //    GameObject::FindComponentWithTag<CameraComponent>(TargetCameraTag);

    //if (const auto sharedEditorCamera = editorCamera.lock())
    //{
    //    sharedEditorCamera->ForceUpdateMatrix();
    //    const Vector3 viewportPos      = sharedEditorCamera->WorldToViewport(transform->Position);
    //    const POINT   absolutePosition = AbsolutePosition;
    //    const POINT   viewportPositionPoint =
    //        POINT{.x = static_cast<LONG>(viewportPos.x), .y = static_cast<LONG>(viewportPos.y)};
    //    _offsetFromTarget = absolutePosition - viewportPositionPoint;
    //}
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
        const POINT   offset      = Offset;
        const SIZE    size        = Size;
        const POINT toCenter{.x = size.cx / 2, .y = size.cy / 2};
        const POINT   toUp{.x = 0, .y = UpDistance};
        Point                     = newPoint - (Offset + margin.LeftTop() + toCenter + toUp);
    }
}
