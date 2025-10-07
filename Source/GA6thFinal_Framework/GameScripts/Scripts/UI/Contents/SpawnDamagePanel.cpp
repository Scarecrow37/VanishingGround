#include "pchScripts.h"
#include "SpawnDamagePanel.h"
#include "DamageElement.h"

UMREAL_COMPONENT(SpawnDamagePanel)

SpawnDamagePanel::SpawnDamagePanel()
{
    FilePath.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                const DragDropAsset::Data* data = static_cast<DragDropAsset::Data*>(payLoad->Data);
                if (const auto extension = data->GetPath().extension(); extension == L".png")
                {
                    _Guid            = data->GetGuid();
                    ReflectFields->Guid = _Guid.string();
                }
            }
            ImGui::EndDragDropTarget();
        }
    });
}

SIZE SpawnDamagePanel::MeasureOverride(const SIZE availableSize)
{
    const FillMode horizontalFillMode = HorizontalFillMode;
    const FillMode verticalFillMode   = VerticalFillMode;
    SIZE           desiredSize        = MinSize()(availableSize, _requestedSize, horizontalFillMode == FillMode::FILL,
                                 verticalFillMode == FillMode::FILL);

    const PADDING padding            = Padding;
    const SIZE    childAvailableSize = desiredSize - padding.Size();
    SIZE          childrenDesiredSize{};
    const std::vector<UIComponent*> children = Children;
    std::ranges::for_each(children, [childAvailableSize, &childrenDesiredSize](UIComponent* child) {
        child->Measure(childAvailableSize);
        const SIZE childDesiredSize = child->DesiredSize;
        childrenDesiredSize         = MaxSize()(childrenDesiredSize, childDesiredSize);
    });

    if (horizontalFillMode == FillMode::WRAP)
        desiredSize.cx = childrenDesiredSize.cx + padding.Horizontal();
    if (verticalFillMode == FillMode::WRAP)
        desiredSize.cy = childrenDesiredSize.cy + padding.Vertical();

    return desiredSize;
}

SIZE SpawnDamagePanel::ArrangeOverride(const SIZE finalSize)
{
    const SIZE    desiredSize        = DesiredSize;
    const SIZE    actualSize         = MinSize()(finalSize, desiredSize);
    const PADDING padding            = Padding;
    const SIZE    childAvailableSize = actualSize - padding.Size();
    const HorizontalAlignment       horizontalAlign    = HorizontalAlign;
    const VerticalAlignment         verticalAlign      = VerticalAlign;
    const POINT                     absoluteChildPosition = AbsolutePosition;

    const std::vector<UIComponent*> children = Children;
    std::ranges::for_each(children, [childAvailableSize, horizontalAlign, verticalAlign,
                                     absoluteChildPosition](UIComponent* child) {
        const SIZE  childDesiredSize = child->DesiredSize;
        const POINT alignPosition = AlignPoint()(horizontalAlign, verticalAlign, childAvailableSize - childDesiredSize);
        const POINT childPoint    = absoluteChildPosition + alignPosition;
        child->Arrange(childPoint, childAvailableSize);
    });

    return actualSize;
}

void SpawnDamagePanel::OnDrawDebugSelectedOverride()
{
    UIComponent::OnDrawDebugSelectedOverride();

    const POINT center = AbsoluteCenterPoint;
    const SIZE  size   = Size;
    const float radius = RadiusRatio * std::min(size.cx, size.cy) * 0.5f;
    DrawDebug()(center, radius, Colors::Red);
}

void SpawnDamagePanel::DeserializedReflectEvent()
{
    UIComponent::DeserializedReflectEvent();

    const File::Guid guid = ReflectFields->Guid;
    if (const auto path = guid.ToPath(); !path.IsNull())
    {
        _Guid = path.ToGuid();
    }
}

void SpawnDamagePanel::Awake()
{
    UIComponent::Awake();

    BindInputAction(ControllerButton::A, Action::PRESSED, this, &SpawnDamagePanel::OnButton);
}

void SpawnDamagePanel::Reset()
{
    UIComponent::Reset();

    EraseChild();
}

void SpawnDamagePanel::EraseChild() const
{
    Transform& transform = this->transform;

    const int                childCount = transform.GetChildCount();
    std::vector<GameObject*> children;
    for (int i = 0; i < childCount; ++i)
    {
        const Transform* childTransform  = transform.GetChild(i);
        GameObject&      childGameObject = childTransform->gameObject;
        children.push_back(&childGameObject);
    }
    for (GameObject* child : children)
    {
        GameObject::Destroy(child);
    }
    children.clear();
}

DamageElement* SpawnDamagePanel::MakeDamage() const
{
    const std::shared_ptr<GameObject> child = NewGameObject(GameObject::Helper::GenerateUniqueName("Damage Element"));
    DamageElement&                    damageElement = child->AddComponent<DamageElement>();
    auto [point, angle]                             = GetRandomSpawnPointAndAngle();
    damageElement.Point                             = point;
    damageElement.Size                              = {50, 20};
    const LONG distance                             = static_cast<LONG>((1 - RadiusRatio) * Radius);
    std::array<std::string, 1> revelations  = {"하늘베기"};
    damageElement.Setup(distance, angle, LifeTime, point, _Guid, FontScale, Color, "100", revelations);
    child->transform->SetParent(transform, true);
    return &damageElement;
}

std::pair<POINT,float> SpawnDamagePanel::GetRandomSpawnPointAndAngle() const
{
    const POINT    center            = CenterPoint;
    const float    randomRadiusRatio = Random::Range(0.0f, RadiusRatio);
    const float    radius            = randomRadiusRatio * Radius;
    const float    angle             = Random::Range(0.0f, XM_2PI);
    const XMVECTOR direction         = XMVectorSet(radius, 0, 0, 0);
    const XMMATRIX rotation          = XMMatrixRotationZ(angle);
    const XMVECTOR vector            = XMVector3Transform(direction, rotation);
    const XMVECTOR centerVector      = XMVectorSet(static_cast<float>(center.x), static_cast<float>(center.y), 0, 0);
    const XMVECTOR    resultVector      = XMVectorAdd(centerVector, vector);
    const POINT       result = {static_cast<LONG>(XMVectorGetX(resultVector)), static_cast<LONG>(XMVectorGetY(resultVector))};
    return std::make_pair(result, angle);
}

void SpawnDamagePanel::OnButton(const Input::Controller& controller)
{
    _damageElements.push_back(MakeDamage());
}