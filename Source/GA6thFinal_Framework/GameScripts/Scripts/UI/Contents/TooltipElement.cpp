#include "pchScripts.h"
#include "TooltipElement.h"

#include "Scripts/UI/Elements/Image/ImageElement.h"
#include "Scripts/UI/Elements/Text/TextElement.h"
#include "Scripts/UI/Panels/Description/DescriptionPanel.h"
#include "TooltipSystem/TooltipSystem.h"
#include "Utility/SingletonHelper.h"

UMREAL_COMPONENT(TooltipElement)

TooltipElement::TooltipElement() = default;

void TooltipElement::SetTooltip(const int id) const
{
    if (const TooltipSystem* tooltipSystem = SingletonComponent<TooltipSystem>::GetInstance())
    {
        const auto [assetId, title, color, description] = tooltipSystem->GetTooltip(id);

        if (const auto sharedIcon = _icon.lock())
        {
            sharedIcon->SetImage(UmFileSystem.GetGuidFromAssetID(assetId));
        }

        if (const auto sharedName = _name.lock())
        {
            sharedName->Text = title;
            sharedName->Color = color;
        }

        if (const auto sharedDescription = _description.lock())
        {
            sharedDescription->Description = description;
        }
    }
}

void TooltipElement::ImGuiDrawPropertysEvent()
{
    HorizontalPanel::ImGuiDrawPropertysEvent();

    static int tooltipId = 0;

    ImGui::InputInt("ID", &tooltipId, 0);
    ImGui::SameLine();
    if (ImGui::Button("Set Tooltip"))
    {
        SetTooltip(tooltipId);
    }

    if (_isDebug)
    {
        
    }
}

void TooltipElement::FindComponents()
{
    Transform::ForeachBFS(transform, [this](const Transform* childTransform, const int depth) {
        if (depth == 1)
        {
            const GameObject& object = childTransform->gameObject;
            if (const ImageElement* iconElement = object.GetComponent<ImageElement>())
            {
                _icon = iconElement->GetWeakPtrAs<ImageElement>();
            }
            else if (const TextElement* nameElement = object.GetComponent<TextElement>())
            {
                _name = nameElement->GetWeakPtrAs<TextElement>();
            }
            else if (const DescriptionPanel* descriptionElement = object.GetComponent<DescriptionPanel>())
            {
                _description = descriptionElement->GetWeakPtrAs<DescriptionPanel>();
            }
        }
    });
}