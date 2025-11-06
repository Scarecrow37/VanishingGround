#include "pchScripts.h"
#include "TooltipComponent.h"
#include "Scripts/UI/Elements/Image/ImageElement.h"
#include "Scripts/UI/Elements/Text/TextElement.h"
#include "Scripts/UI/Panels/Description/DescriptionPanel.h"

UMREAL_COMPONENT(TooltipComponent)

TooltipComponent::TooltipComponent() = default;

void TooltipComponent::SetTooltip(const Tooltip::TooltipData& data) const
{
    if (const auto image = _image.lock())
    {
        if (const File::Guid guid = UmFileSystem.GetGuidFromAssetID(data.ImageAssetId); !guid.empty())
        {
            image->SetImage(guid);
        }
        else
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING,
                         std::format("Fail to find image asset with id: {}", data.ImageAssetId));
        }
    }

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
    GameObject& object = gameObject;
    object.SetActive(true);
}

void TooltipComponent::Hide() const
{
    GameObject& object = gameObject;
    object.SetActive(false);
}

void TooltipComponent::Awake()
{
    Component::Awake();

    FindComponents();
}

void TooltipComponent::ImGuiDrawPropertysEvent()
{
    Component::ImGuiDrawPropertysEvent();

    static Tooltip::TooltipData data = {};
    ImGui::InputInt("Image Asset Id", &data.ImageAssetId);
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

void TooltipComponent::FindComponents()
{
    Transform::ForeachBFS(transform, [this](const Transform* childTransform, const int depth) {
        if (depth == 1)
        {
            const GameObject& childObject = childTransform->gameObject;
            if (const ImageElement* imageComponentRaw = childObject.GetComponent<ImageElement>())
            {
                _image = imageComponentRaw->GetWeakPtrAs<ImageElement>();
            }
            else if (const TextElement* textComponentRaw = childObject.GetComponent<TextElement>())
            {
                _title = textComponentRaw->GetWeakPtrAs<TextElement>();
            }
            else if (const DescriptionPanel* descriptionComponentRaw = childObject.GetComponent<DescriptionPanel>())
            {
                _description = descriptionComponentRaw->GetWeakPtrAs<DescriptionPanel>();
            }
        }
    });
}

