#include "pchScripts.h"
#include "PaddingWrapper.h"

void PaddingWrapper::OnPlacementChange()
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
                POINT point = ReflectFields->Basefields.get().Basefields.get().Point;
                auto [scopeX, scopeY] = ReflectFields->Basefields.get().Basefields.get().ScopePoint;
                point.x += scopeX;
                point.y += scopeY;
                point.x += ReflectFields->PadLeft;
                point.y += ReflectFields->PadTop;
                SIZE size = ReflectFields->Basefields.get().Basefields.get().Size;
                size.cx -= (ReflectFields->PadLeft + ReflectFields->PadRight);
                size.cy -= (ReflectFields->PadTop + ReflectFields->PadBottom);
                component->SetScopePlacement(point, size);
            }
        }
    }
}