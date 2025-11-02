#include "pchScripts.h"
#include "FadeUIComponent.h"

#include "UI/Elements/Image/ImageElement.h"
#include "UI/Elements/Text/TextElement.h"
#include "UI/Panels/Description/DescriptionPanel.h"

UMREAL_COMPONENT(FadeUIComponent)

FadeUIComponent::FadeUIComponent()
    : UIAnimation([this](const float alpha) { UpdateOpacity(alpha); }), _fadeDirection(FadeDirection::NONE)
{
}

void FadeUIComponent::FadeIn()
{
    _fadeDirection = FadeDirection::FORWARD;
    UIAnimation::Reset();
}

void FadeUIComponent::FadeOut()
{
    _fadeDirection = FadeDirection::BACKWARD;
    UIAnimation::Reset();
}

void FadeUIComponent::Stop()
{
    _fadeDirection = FadeDirection::NONE;
}

void FadeUIComponent::Begin()
{
    SetElapsedTime(0.0f);
}

void FadeUIComponent::End()
{
    const float duration = FadeDuration;
    SetElapsedTime(duration);
}

bool FadeUIComponent::IsComplete() const
{
    if (_fadeDirection == FadeDirection::NONE)
    {
        return true;
    }
    if (_fadeDirection == FadeDirection::FORWARD)
    {
        return GetElapsedTime() >= ReflectFields->FadeDuration;
    }
    if (_fadeDirection == FadeDirection::BACKWARD)
    {
        return GetElapsedTime() <= 0.0f;
    }
    return false;
}

void FadeUIComponent::CompleteImmediately()
{
    if (_fadeDirection == FadeDirection::FORWARD)
    {
        End();
        UpdateOpacity(1.0f);
        _fadeDirection = FadeDirection::NONE;
    }
}

void FadeUIComponent::Update()
{
    Component::Update();

    switch (_fadeDirection)
    {
    case FadeDirection::NONE:
        break;
    case FadeDirection::FORWARD:
        UIAnimation::Update(UmTime.DeltaTime());
        break;
    case FadeDirection::BACKWARD:
        UIAnimation::Update(-UmTime.DeltaTime());
        break;
    }
}

void FadeUIComponent::Reset()
{
    Component::Reset();

    _fadeDirection = FadeDirection::NONE;
}

void FadeUIComponent::Awake()
{
    Component::Awake();

    UpdateAnimationProperty();

    ClearTargets();
    FindTargets();
}

void FadeUIComponent::ImGuiDrawPropertysEvent()
{
    Component::ImGuiDrawPropertysEvent();

    if (ImGui::Button("Fade In"))
    {
        FadeIn();
    }

    if (ImGui::Button("Fade Out"))
    {
        FadeOut();
    }

    if (ImGui::Button("Stop"))
    {
        Stop();
    }

    if (ImGui::Button("Begin"))
    {
        Begin();
    }

    if (ImGui::Button("End"))
    {
        End();
    }
}

void FadeUIComponent::UpdateOpacity(const float value)
{
    float opacity = std::lerp(BeginOpacity, EndOpacity, value);
    std::ranges::for_each(_targets, [opacity](const std::weak_ptr<IOpacity>& weakTarget) {
        if (const std::shared_ptr<IOpacity> target = weakTarget.lock(); nullptr != target)
        {
            target->SetOpacity(opacity);
        }
    });
}

void FadeUIComponent::ClearTargets()
{
    _targets.clear();
}

void FadeUIComponent::FindTargets()
{
    Transform& myTransform = transform;
    Transform::ForeachBFS(myTransform, [this](const Transform* transform) {
        const GameObject& object = transform->gameObject;
        if (const TextElement* textElement = object.GetComponentDynamic<TextElement>();
            nullptr != textElement && false == textElement->IsArtificial())
        {
            _targets.push_back(textElement->GetWeakPtrAs<IOpacity>());
            return;
        }
        if (const ImageElement* imageElement = object.GetComponentDynamic<ImageElement>();
            nullptr != imageElement && false == imageElement->IsArtificial())
        {
            _targets.push_back(imageElement->GetWeakPtrAs<IOpacity>());
            return;
        }
        if (const DescriptionPanel* descriptionPanel = object.GetComponentDynamic<DescriptionPanel>())
        {
            _targets.push_back(descriptionPanel->GetWeakPtrAs<IOpacity>());
            return;
        }
    });
}

void FadeUIComponent::UpdateAnimationProperty()
{
    const float duration = ReflectFields->FadeDuration;
    SetDuration(duration);
}