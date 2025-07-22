#include "pchScripts.h"
#include "AnchorPanel.h"

AnchorPanel::AnchorPanel() = default;

void AnchorPanel::OnAttachChild(GameObject* childGameObject)
{
    EditablePlacementUIComponent::OnAttachChild(childGameObject);
    auto& slot = childGameObject->AddComponent<AnchorPanelSlot>();
    slot.SetPlacement(ReflectFields->Basefields.get().Basefields.get().Point,
                      ReflectFields->Basefields.get().Basefields.get().Size);
}

void AnchorPanel::OnPlacementChange()
{
    EditablePlacementUIComponent::OnPlacementChange();
    for (int i = 0; i < transform->GetChildCount(); ++i)
    {
        const Transform* child      = transform->GetChild(i);
        GameObject&      gameObject = child->gameObject;
        for (int j = 0; j < gameObject.GetComponentCount(); ++j)
        {
            if (AnchorPanelSlot* slot = gameObject.GetComponentAtIndex<AnchorPanelSlot>(j))
            {
                POINT point = ReflectFields->Basefields.get().Basefields.get().Point;
                auto [x, y] = ReflectFields->Basefields.get().Basefields.get().ScopePoint;
                point.x += x;
                point.y += y;
                slot->SetPlacement(point,
                                   ReflectFields->Basefields.get().Basefields.get().Size);
            }
        }
    }
}

AnchorPanelSlot::AnchorPanelSlot() = default;

void AnchorPanelSlot::OnSetPlacement()
{
    const POINT point         = ReflectFields->Basefields.get().Basefields.get().Point;
    const auto size = ReflectFields->Basefields.get().Basefields.get().Size;

    ReflectFields->AnchorPoint = point;
    switch (AnchorType anchor = ReflectFields->Anchor)
    {
    case AnchorType::TOP_LEFT:
        // Nothing to do here, as the point is already at the top left.
        break;
    case AnchorType::TOP_CENTER:
        ReflectFields->AnchorPoint.x += size.cx / 2;
        break;
    case AnchorType::TOP_RIGHT:
        ReflectFields->AnchorPoint.x += size.cx;
        break;
    case AnchorType::CENTER_LEFT:
        ReflectFields->AnchorPoint.y += size.cy / 2;
        break;
    case AnchorType::CENTER:
        ReflectFields->AnchorPoint.x += size.cx / 2;
        ReflectFields->AnchorPoint.y += size.cy / 2;
        break;
    case AnchorType::CENTER_RIGHT:
        ReflectFields->AnchorPoint.x += size.cx;
        ReflectFields->AnchorPoint.y += size.cy / 2;
        break;
    case AnchorType::BOTTOM_LEFT:
        ReflectFields->AnchorPoint.y += size.cy;
        break;
    case AnchorType::BOTTOM_CENTER:
        ReflectFields->AnchorPoint.x += size.cx / 2;
        ReflectFields->AnchorPoint.y += size.cy;
        break;
    case AnchorType::BOTTOM_RIGHT:
        ReflectFields->AnchorPoint.x += size.cx;
        ReflectFields->AnchorPoint.y += size.cy;
        break;
    }

    PassScopedPlacement(ReflectFields->AnchorPoint, size);
}

void AnchorPanelSlot::ImGuiDrawPropertysEvent()
{
    PanelSlotComponent::ImGuiDrawPropertysEvent();
    static const char* items[]      = {"Top Left",     "Top Center",  "Top Right",     "Center Left", "Center",
                                       "Center Right", "Bottom Left", "Bottom Center", "Bottom Right"};
    int                currentIndex = static_cast<int>(ReflectFields->Anchor);
    if (ImGui::Combo("##Anchor", &currentIndex, items, IM_ARRAYSIZE(items)))
    {
        ReflectFields->Anchor = static_cast<AnchorType>(currentIndex);
        OnSetPlacement();
    }
    ImGui::SameLine();
    ImGui::Text("Anchor");
}

void AnchorPanelSlot::DrawDebugSelected()
{
    PanelSlotComponent::DrawDebugSelected();

    auto [anchorX, anchorY] = ReflectFields->AnchorPoint;

    constexpr LONG lineSize = 20;

    // Top to bottom line
    {
        POINT start{anchorX, anchorY - lineSize};
        POINT end{anchorX, anchorY + lineSize};
        UmDebugDrawCore.Draw("Editor", {start, end}, DirectX::Colors::Yellow);
    }

    // Left to right line
    {
        POINT start{anchorX - lineSize, anchorY};
        POINT end{anchorX + lineSize, anchorY};
        UmDebugDrawCore.Draw("Editor", {start, end}, DirectX::Colors::Yellow);
    }

    // LeftTop to RightBottom line
    {
        POINT start{anchorX - lineSize, anchorY - lineSize};
        POINT end{anchorX + lineSize, anchorY + lineSize};
        UmDebugDrawCore.Draw("Editor", {start, end}, DirectX::Colors::Yellow);
    }

    // RightTop to LeftBottom line
    {
        POINT start{anchorX + lineSize, anchorY - lineSize};
        POINT end{anchorX - lineSize, anchorY + lineSize};
        UmDebugDrawCore.Draw("Editor", {start, end}, DirectX::Colors::Yellow);
    }
}