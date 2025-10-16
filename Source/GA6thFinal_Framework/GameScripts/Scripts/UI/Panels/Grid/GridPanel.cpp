#include "pchScripts.h"
#include "GridPanel.h"


UMREAL_COMPONENT(GridPanel)
UMREAL_COMPONENT(GridPanelSlot)

GridPanelSlot::GridPanelSlot() = default;

void GridPanelSlot::ImGuiDrawPropertysEvent()
{
    SlotComponent::ImGuiDrawPropertysEvent();

    if (_isDebug)
    {
        const unsigned int columns = ReflectFields->Columns;
        ImGuiDebug()("Columns", columns);
        const unsigned int rows = ReflectFields->Rows;
        ImGuiDebug()("Rows", rows);
    }
}

void GridPanelSlot::SetColumnsAndRows(const unsigned int columns, const unsigned int rows)
{
    ReflectFields->Columns = std::clamp(columns, GridPanel::MIN_COLUMNS, GridPanel::MAX_COLUMNS);
    ReflectFields->Rows    = std::clamp(rows, GridPanel::MIN_ROWS, GridPanel::MAX_ROWS);
}

GridPanel::GridPanel() = default;

void GridPanel::OnAttachChild(GameObject* childGameObject)
{
    UIComponent::OnAttachChild(childGameObject);

    auto& slot = childGameObject->AddComponent<GridPanelSlot>();
    slot.SetColumnsAndRows(Columns, Rows);
}

void GridPanel::OnDrawDebugOverride()
{
    UIComponent::OnDrawDebugOverride();

    DrawSubline(1, Colors::White);
}

void GridPanel::OnDrawDebugSelectedOverride()
{
    UIComponent::OnDrawDebugSelectedOverride();

    DrawSubline(3, Colors::Yellow);
}

SIZE GridPanel::MeasureOverride(const SIZE availableSize)
{
    const FillMode horizontalFillMode = HorizontalFillMode;
    const FillMode verticalFillMode   = VerticalFillMode;

    // Wrap 모드는 없음
    if (horizontalFillMode == FillMode::WRAP)
        HorizontalFillMode = FillMode::NONE;
    if (verticalFillMode == FillMode::WRAP)
        VerticalFillMode = FillMode::NONE;

    const SIZE desiredSize = MinSize()(availableSize, _requestedSize, horizontalFillMode == FillMode::FILL,
                                       verticalFillMode == FillMode::FILL);

    const unsigned int columns     = Columns;
    const unsigned int rows        = Rows;
    const LONG cellWidth  = desiredSize.cx / static_cast<LONG>(columns);
    const LONG cellHeight = desiredSize.cy / static_cast<LONG>(rows);

    const PADDING padding = Padding;

    const std::vector<UIComponent*> children = Children;
    std::ranges::for_each(children, [ cellWidth, cellHeight, padding ](UIComponent* child) {
        if (const GridPanelSlot* slot = child->GetComponent<GridPanelSlot>(); nullptr != slot)
        {
            const unsigned int colSpan = slot->ColumnSpan;
            const unsigned int rowSpan = slot->RowSpan;

            const SIZE cellSize{.cx = cellWidth * static_cast<LONG>(colSpan),
                                .cy = cellHeight * static_cast<LONG>(rowSpan)};

            const SIZE childAvailableSize = cellSize - padding.Size();
            child->Measure(childAvailableSize);
        }
    });

    return desiredSize;
}

SIZE GridPanel::ArrangeOverride(const SIZE finalSize)
{
    const SIZE desiredSize = DesiredSize;
    const SIZE actualSize  = MinSize()(finalSize, desiredSize);

    const unsigned int columns    = Columns;
    const unsigned int rows       = Rows;
    const LONG         cellWidth  = actualSize.cx / static_cast<LONG>(columns);
    const LONG         cellHeight = actualSize.cy / static_cast<LONG>(rows);

    const PADDING             padding         = Padding;
    const HorizontalAlignment horizontalAlign = HorizontalAlign;
    const VerticalAlignment   verticalAlign   = VerticalAlign;

    const std::vector<UIComponent*> children = Children;
    std::ranges::for_each(children, [this, horizontalAlign, verticalAlign, cellWidth, cellHeight, padding](UIComponent* child) {
        if (const GridPanelSlot* slot = child->GetComponent<GridPanelSlot>(); nullptr != slot)
        {
            const unsigned int colSpan = slot->ColumnSpan;
            const unsigned int rowSpan = slot->RowSpan;

            const SIZE cellSize{.cx = cellWidth * static_cast<LONG>(colSpan),
                                .cy = cellHeight * static_cast<LONG>(rowSpan)};

            const SIZE childAvailableSize = cellSize - padding.Size();

            const SIZE         childSize     = child->DesiredSize;
            const unsigned int column  = slot->Column;
            const unsigned int row     = slot->Row;

            const POINT absolutePosition = AbsoluteChildPosition;
            const POINT cellPosition  = {.x = cellWidth * static_cast<LONG>(column),
                                         .y = cellHeight * static_cast<LONG>(row)};
            const POINT alignedPosition    = AlignPoint()(horizontalAlign, verticalAlign, childAvailableSize - childSize);
            const POINT childPosition = absolutePosition + cellPosition + alignedPosition;

            child->Arrange(childPosition, childAvailableSize);
        }
    });

    return actualSize;
}

void GridPanel::DrawSubline(const int thickness, FXMVECTOR color) const
{
    constexpr DrawDebug drawDebug;
    const unsigned int  columns = Columns;
    const unsigned int  rows    = Rows;
    if (columns == 0 || rows == 0)
        return;

    const POINT absolutePosition      = AbsolutePosition;
    const auto [absoluteX, absoluteY] = absolutePosition;
    const SIZE size                   = Size;

    const long stepX = size.cx / static_cast<LONG>(columns);
    for (unsigned int i = 1; i < columns; ++i)
    {
        const POINT start{.x = absoluteX + stepX * static_cast<LONG>(i), .y = absoluteY};
        const POINT end{.x = start.x, .y = start.y + size.cy};

        drawDebug(start, end, thickness, true, color);
    }

    const long stepY = size.cy / static_cast<LONG>(rows);
    for (unsigned int i = 1; i < rows; ++i)
    {
        const POINT start{.x = absoluteX, .y = absoluteY + stepY * static_cast<LONG>(i)};
        const POINT end{.x = start.x + size.cx, .y = start.y};

        drawDebug(start, end, thickness, false, color);
    }
}

void GridPanel::PassRowsAndColumns() const
{
    std::vector<GridPanelSlot*> slots = Slots;
    std::ranges::for_each(slots, [this](GridPanelSlot* slot) { slot->SetColumnsAndRows(Columns, Rows); });
}

std::vector<GridPanelSlot*> GridPanel::GetSlots(const GameObject& parentGameObject)
{
    return parentGameObject.GetComponents<GridPanelSlot>();
}