#include "pchScripts.h"
#include "GridPanel.h"

GridPanel::GridPanel() = default;

void GridPanel::OnAttachChild(GameObject* childGameObject)
{
    UIComponent::OnAttachChild(childGameObject);
    childGameObject->AddComponent<GridPanelSlot>();
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

void GridPanel::DrawLine(FXMVECTOR color) const
{
    const unsigned int columns = Columns;
    const unsigned int rows    = Rows;
    if (columns == 0 || rows == 0) return;
    

    const long stepX = _size.cx / static_cast<LONG>(columns);
    for (unsigned int i = 1; i < columns; ++i)
    {
        POINT start{};
        start.x = _point.x + stepX * static_cast<LONG>(i);
        start.y = _point.y;

        POINT end{};
        end.x = start.x;
        end.y = start.y + _size.cy;

        UmDebugDrawCore.Draw("Editor", {start, end}, color);
    }

    const long stepY = _size.cy / static_cast<LONG>(rows);
    for (unsigned int i = 1; i < rows; ++i)
    {
        POINT start{};
        start.x = _point.x;
        start.y = _point.y + stepY * static_cast<LONG>(i);
        POINT end{};
        end.x = start.x + _size.cx;
        end.y = start.y;
        UmDebugDrawCore.Draw("Editor", {start, end}, color);
    }
}