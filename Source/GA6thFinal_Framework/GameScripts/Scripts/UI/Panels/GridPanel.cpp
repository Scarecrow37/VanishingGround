#include "pchScripts.h"
#include "GridPanel.h"

GridPanel::GridPanel() = default;

void GridPanel::OnAttachChild(GameObject* childGameObject)
{
    UIComponent::OnAttachChild(childGameObject);
    auto& slot = childGameObject->AddComponent<GridPanelSlot>();
    slot.SetGridPanel(this);
}

void GridPanel::OnDrawDebug()
{
    AreaUIComponent::OnDrawDebug();
    DrawLine(DirectX::Colors::White);
}

void GridPanel::OnDrawDebugSelected()
{
    AreaUIComponent::OnDrawDebugSelected();
    DrawLine(DirectX::Colors::Yellow);
}

void GridPanel::Reset()
{
    AreaUIComponent::Reset();
    for (int i = 0; i < transform->GetChildCount(); ++i)
    {
        if (const auto* child = transform->GetChild(i))
        {
            if (auto* slot = child->gameObject->GetComponent<GridPanelSlot>())slot->SetGridPanel(this);
        }
    }
}

void GridPanel::DrawLine(FXMVECTOR color) const
{
    const unsigned int columns = Columns;
    const unsigned int rows    = Rows;
    const SIZE         size    = Size;
    const POINT        point   = Point;
    if (columns == 0 || rows == 0)
        return;

    const long stepX = size.cx / static_cast<LONG>(columns);
    for (unsigned int i = 1; i < columns; ++i)
    {
        POINT start{};
        start.x = point.x + stepX * static_cast<LONG>(i);
        start.y = point.y;

        POINT end{};
        end.x = start.x;
        end.y = start.y + size.cy;

        UmDebugDrawCore.Draw("Editor", {start, end}, color);
    }

    const long stepY = size.cy / static_cast<LONG>(rows);
    for (unsigned int i = 1; i < rows; ++i)
    {
        POINT start{};
        start.x = point.x;
        start.y = point.y + stepY * static_cast<LONG>(i);
        POINT end{};
        end.x = start.x + size.cx;
        end.y = start.y;
        UmDebugDrawCore.Draw("Editor", {start, end}, color);
    }
}

GridPanelSlot::GridPanelSlot() = default;

void GridPanelSlot::SetGridPanel(GridPanel* gridPanel)
{
    _gridPanel = gridPanel;
}