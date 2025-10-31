#include "pchScripts.h"
#include "TooltipComponent.h"
#include "Scripts/UI/Elements/Text/TextElement.h"
#include "Scripts/UI/Panels/Description/DescriptionPanel.h"

UMREAL_COMPONENT(TooltipComponent)

TooltipComponent::TooltipComponent() = default;

void TooltipComponent::SetTooltip(const TooltipData& data) const
{
    if (const auto title = _title.lock())
    {
        title->Text  = data.Title;
    }

    if (const auto description = _description.lock())
    {
        description->Description = data.Description;
    }
}

void TooltipComponent::Show() const
{
    if (const auto panel = _panel.lock())
    {
        panel->SetActive(true);
    }
}

void TooltipComponent::Hide() const
{
    if (const auto panel = _panel.lock())
    {
        panel->SetActive(false);
    }
}

void TooltipComponent::Awake()
{
    Component::Awake();

    FindComponent();
}

void TooltipComponent::Start()
{
    Component::Start();

    Hide();
}

void TooltipComponent::ImGuiDrawPropertysEvent()
{
    Component::ImGuiDrawPropertysEvent();

    static TooltipData data = {};
    ImGui::InputText("Title", &data.Title);
    ImGui::InputText("Description", &data.Description);


    if (ImGui::Button("Set Tooltip"))
    {
        SetTooltip(data);
    }

    if (ImGui::Button("Show Tooltip"))
    {
        Show();
    }

    if (ImGui::Button("Hide Tooltip"))
    {
        Hide();
    }
}

void TooltipComponent::FindComponent()
{
    _panel = gameObject->GetWeakPtr();

    if (_panel.expired())
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, "Fail to find tooltip panel object.");
    }

    if (const Transform* titleTransform = transform->FindWithTag(TAG_TITLE_COMPONENT))
    {
        const GameObject&  titleObject       = titleTransform->gameObject;
        if (const TextElement* titleComponentRaw = titleObject.GetComponentDynamic<TextElement>())
        {
            _title = titleComponentRaw->GetWeakPtrAs<TextElement>();
        }
    }

    if (_title.expired())
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, "Fail to find tooltip title text component.");
    }

    if (const Transform* descriptionTransform = transform->FindWithTag(TAG_DESCRIPTION_COMPONENT))
    {
        const GameObject&      descriptionObject = descriptionTransform->gameObject;
        if (const DescriptionPanel* descriptionComponentRaw = descriptionObject.GetComponentDynamic<DescriptionPanel>())
        {
            _description = descriptionComponentRaw->GetWeakPtrAs<DescriptionPanel>();
        }
    }

    if (_description.expired())
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, "Fail to find tooltip description panel component.");
    }
}

