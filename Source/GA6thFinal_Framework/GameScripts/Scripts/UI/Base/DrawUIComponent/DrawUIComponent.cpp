#include "pchScripts.h"
#include "DrawUIComponent.h"

REFLECT_FUNCTION(DrawUIComponent)

DrawUIComponent::DrawUIComponent() = default;

void DrawUIComponent::SetViewOrder(const int viewOrder)
{
    ReflectFields->ViewOrder = viewOrder;
}

void DrawUIComponent::SetArtificial(bool isArtificial)
{
    _isArtificial = isArtificial;
}

bool DrawUIComponent::IsArtificial() const
{
    return _isArtificial;
}

void DrawUIComponent::ImGuiDrawPropertysEvent()
{
    UIComponent::ImGuiDrawPropertysEvent();

    if (_isDebug)
    {
        const int viewOrder = ReflectFields->ViewOrder;
        ImGuiDebug()("View Order", viewOrder);
    }
}

float DrawUIComponent::GetZOrder() const
{
    return static_cast<float>(ReflectFields->ViewOrder);
}