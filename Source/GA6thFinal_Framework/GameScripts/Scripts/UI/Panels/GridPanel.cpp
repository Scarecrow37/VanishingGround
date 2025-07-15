#include "pchScripts.h"
#include "GridPanel.h"

GridPanel::GridPanel() = default;

void GridPanel::OnAttachChild(GameObject* childGameObject)
{
    EditablePlacementUIComponent::OnAttachChild(childGameObject);
    auto& slot = childGameObject->AddComponent<GridPanelSlot>();
    slot.SetColumnsAndRows(ReflectFields->Columns, ReflectFields->Rows);
    slot.SetPlacement(ReflectFields->Basefields.get().Basefields.get().Point,
                      ReflectFields->Basefields.get().Basefields.get().Size);
}

void GridPanel::DrawDebug()
{
    EditablePlacementUIComponent::DrawDebug();
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

        UmDebugDrawCore.Draw("Editor", {start, end}, DirectX::Colors::White);
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
        UmDebugDrawCore.Draw("Editor", {start, end}, DirectX::Colors::White);
    }
}

void GridPanel::DrawDebugSelected()
{
    EditablePlacementUIComponent::DrawDebugSelected();
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

        UmDebugDrawCore.Draw("Editor", {start, end}, DirectX::Colors::Yellow);

        POINT leftStart{start.x - 1, start.y};
        POINT leftEnd{start.x - 1, end.y};
        UmDebugDrawCore.Draw("Editor", {leftStart, leftEnd}, DirectX::Colors::Yellow);

        POINT rightStart{end.x + 1, start.y};
        POINT rightEnd{end.x + 1, end.y};
        UmDebugDrawCore.Draw("Editor", {rightStart, rightEnd}, DirectX::Colors::Yellow);
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
        UmDebugDrawCore.Draw("Editor", {start, end}, DirectX::Colors::Yellow);

        POINT upStart{start.x, start.y - 1};
        POINT upEnd{end.x, start.y - 1};
        UmDebugDrawCore.Draw("Editor", {upStart, upEnd}, DirectX::Colors::Yellow);

        POINT downStart{start.x, end.y + 1};
        POINT downEnd{end.x, end.y + 1};
        UmDebugDrawCore.Draw("Editor", {downStart, downEnd}, DirectX::Colors::Yellow);
    }
}

void GridPanel::OnPlacementChange()
{
    EditablePlacementUIComponent::OnPlacementChange();
    for (int i = 0; i < transform->GetChildCount(); ++i)
    {
        const Transform* child = transform->GetChild(i);
        GameObject& gameObject = child->gameObject;
        for (int j = 0; j < gameObject.GetComponentCount(); ++j)
        {
            if (GridPanelSlot* slot = gameObject.GetComponentAtIndex<GridPanelSlot>(j))
            {
                slot->SetPlacement(ReflectFields->Basefields.get().Basefields.get().Point,
                                  ReflectFields->Basefields.get().Basefields.get().Size);
            }
        }
    }
}

GridPanelSlot::GridPanelSlot() = default;

void GridPanelSlot::PassPlacement() const
{
    const unsigned int columns = ReflectFields->Columns;
    const unsigned int rows    = ReflectFields->Rows;
    if (columns == 0 || rows == 0)
        return;

    const auto [pointX, pointY] = ReflectFields->Basefields.get().Basefields.get().Point;
    const auto [sizeX, sizeY]   = ReflectFields->Basefields.get().Basefields.get().Size;
    const long stepX            = sizeX / static_cast<LONG>(columns);
    const long stepY            = sizeY / static_cast<LONG>(rows);

    const POINT scopePoint{.x = pointX + stepX * static_cast<LONG>(ReflectFields->Column),
                           .y = pointY + stepY * static_cast<LONG>(ReflectFields->Row)};
    const SIZE  scopeSize{.cx = stepX * static_cast<LONG>(ReflectFields->ColumnSpan),
                          .cy = stepY * static_cast<LONG>(ReflectFields->RowSpan)};

    PanelSlotComponent::PassPlacement(scopePoint, scopeSize);
}

void GridPanelSlot::SetColumnsAndRows(const unsigned int columns, const unsigned int rows)
{
    SetColumns(columns);
    SetRows(rows);
}

void GridPanelSlot::SetColumns(const unsigned int columns)
{
    ReflectFields->Columns    = std::clamp(columns, GridPanel::MIN_COLUMNS, GridPanel::MAX_COLUMNS);
    Column     = ReflectFields->Column;
}

void GridPanelSlot::SetRows(const unsigned int rows)
{
    ReflectFields->Rows    = std::clamp(rows, GridPanel::MIN_ROWS, GridPanel::MAX_ROWS);
    Row     = ReflectFields->Row;
}