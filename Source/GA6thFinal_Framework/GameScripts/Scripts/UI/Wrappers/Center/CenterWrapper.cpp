#include "pchScripts.h"
#include "CenterWrapper.h"

void CenterWrapper::OnPlacementChange()
{
    EditablePlacementUIComponent::OnPlacementChange();
    for (int i = 0; i < transform->GetChildCount(); ++i)
    {
        const Transform* child      = transform->GetChild(i);
        GameObject&      gameObject = child->gameObject;
        for (int j = 0; j < gameObject.GetComponentCount(); ++j)
        {
            if (PlacementUIComponent* component = gameObject.GetComponentAtIndex<PlacementUIComponent>(j))
            {
                SIZE  childSize       = component->GetSize();
                POINT point           = ReflectFields->Basefields.get().Basefields.get().Point;
                SIZE  size            = ReflectFields->Basefields.get().Basefields.get().Size;
                auto [scopeX, scopeY] = ReflectFields->Basefields.get().Basefields.get().ScopePoint;
                point.x += scopeX;
                point.y += scopeY;
                switch (ReflectFields->CenterType)
                {
                case CenterType::BOTH:
                    point.x += (size.cx - childSize.cx) / 2;
                    point.y += (size.cy - childSize.cy) / 2;
                    break;
                case CenterType::HORIZONTAL:
                    point.x += (size.cx - childSize.cx) / 2;
                    break;
                case CenterType::VERTICAL:
                    point.y += (size.cy - childSize.cy) / 2;
                    break;
                }
                component->SetScopePlacement(point, size);
            }
        }
    }
}

void CenterWrapper::ImGuiDrawPropertysEvent()
{
    EditablePlacementUIComponent::ImGuiDrawPropertysEvent();
    static const char* items[]      = {"Both", "Horizontal", "Vertical"};
    int                currentIndex = static_cast<int>(ReflectFields->CenterType);
    if (ImGui::Combo("##CenterType", &currentIndex, items, IM_ARRAYSIZE(items)))
    {
        ReflectFields->CenterType = static_cast<CenterType>(currentIndex);
        OnPlacementChange();
    }
    ImGui::SameLine();
    ImGui::Text("Center Type");
}