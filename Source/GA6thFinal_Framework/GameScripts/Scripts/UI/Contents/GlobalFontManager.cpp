#include "pchScripts.h"
#include "GlobalFontManager.h"

#include "UI/Elements/Text/TextElement.h"
#include "UI/Panels/Description/DescriptionPanel.h"

UMREAL_COMPONENT(GlobalFontManager)

GlobalFontManager::GlobalFontManager() = default;

void GlobalFontManager::Awake()
{
    Component::Awake();

    FindFontAppearances();
    UpdateFontWeight();
}

void GlobalFontManager::ReFindFontAppearances()
{
    ClearFontAppearances();
    FindFontAppearances();
}

void GlobalFontManager::FindFontAppearances()
{
    Transform& myTransform = transform;
    Transform::ForeachBFS(myTransform, [this](const Transform* transform) {
        const GameObject& object = transform->gameObject;
        if (const TextElement* textElement = object.GetComponentDynamic<TextElement>();
            nullptr != textElement && false == textElement->IsArtificial())
        {
            _fontAppearances.push_back(textElement->GetWeakPtrAs<IFontAppearance>());
            return;
        }
        if (const DescriptionPanel* descriptionPanel = object.GetComponentDynamic<DescriptionPanel>())
        {
            _fontAppearances.push_back(descriptionPanel->GetWeakPtrAs<IFontAppearance>());
            return;
        }
    });
}

void GlobalFontManager::ClearFontAppearances()
{
    _fontAppearances.clear();
}

void GlobalFontManager::UpdateFontWeight()
{
    for (auto fontAppearance : _fontAppearances)
    {
        if (const auto sharedFont = fontAppearance.lock())
        {
            sharedFont->SetFontWeight(ReflectFields->FontWeight);
        }
    }
}
