#include "pchScripts.h"
#include "GridPanel.h"

GridPanel::GridPanel() = default;

unsigned int GridPanel::GetColumns() const
{
    return ReflectFields->Columns;
}

unsigned int GridPanel::GetRows() const
{
    return ReflectFields->Rows;
}

void GridPanel::OnAttachChild(GameObject* childGameObject)
{
    EditablePlacementUIComponent::OnAttachChild(childGameObject);

    auto& slot = childGameObject->AddComponent<GridPanelSlot>();
    AssignChild(slot);
}

void GridPanel::DrawDebug()
{
    EditablePlacementUIComponent::DrawDebug();

    const SublineCallback callback = [](const POINT& start, const POINT& end) {

        const XMFLOAT2 startVector = {static_cast<float>(start.x), static_cast<float>(start.y)};
        const XMFLOAT2 endVector   = {static_cast<float>(end.x), static_cast<float>(end.y)};

        UmGraphics.DebugDraw2D("Editor", XMLoadFloat2(&startVector), XMLoadFloat2(&endVector), Colors::White);
    };

    DrawSubline(callback, callback);
}

void GridPanel::DrawDebugSelected()
{
    EditablePlacementUIComponent::DrawDebugSelected();

    const SublineCallback column = [](const POINT& start, const POINT& end) {
        const XMFLOAT2 startVector = {static_cast<float>(start.x), static_cast<float>(start.y)};
        const XMFLOAT2 endVector   = {static_cast<float>(end.x), static_cast<float>(end.y)};
        UmGraphics.DebugDraw2D("Editor", XMLoadFloat2(&startVector), XMLoadFloat2(&endVector), Colors::Yellow);

        const POINT    leftStart{start.x - 1, start.y};
        const POINT    leftEnd{start.x - 1, end.y};
        const XMFLOAT2 leftStartVector = {static_cast<float>(leftStart.x), static_cast<float>(leftStart.y)};
        const XMFLOAT2 leftEndVector   = {static_cast<float>(leftEnd.x), static_cast<float>(leftEnd.y)};
        UmGraphics.DebugDraw2D("Editor", XMLoadFloat2(&leftStartVector), XMLoadFloat2(&leftEndVector), Colors::Yellow);

        const POINT    rightStart{end.x + 1, start.y};
        const POINT    rightEnd{end.x + 1, end.y};
        const XMFLOAT2 rightStartVector = {static_cast<float>(rightStart.x), static_cast<float>(rightStart.y)};
        const XMFLOAT2 rightEndVector   = {static_cast<float>(rightEnd.x), static_cast<float>(rightEnd.y)};
        UmGraphics.DebugDraw2D("Editor", XMLoadFloat2(&rightStartVector), XMLoadFloat2(&rightEndVector),
                               Colors::Yellow);
    };

    const SublineCallback row = [](const POINT& start, const POINT& end) {
        const XMFLOAT2 startVector = {static_cast<float>(start.x), static_cast<float>(start.y)};
        const XMFLOAT2 endVector   = {static_cast<float>(end.x), static_cast<float>(end.y)};
        UmGraphics.DebugDraw2D("Editor", XMLoadFloat2(&startVector), XMLoadFloat2(&endVector), Colors::Yellow);

        const POINT    upStart{start.x, start.y - 1};
        const POINT    upEnd{end.x, start.y - 1};
        const XMFLOAT2 upStartVector = {static_cast<float>(upStart.x), static_cast<float>(upStart.y)};
        const XMFLOAT2 upEndVector   = {static_cast<float>(upEnd.x), static_cast<float>(upEnd.y)};
        UmGraphics.DebugDraw2D("Editor", XMLoadFloat2(&upStartVector), XMLoadFloat2(&upEndVector), Colors::Yellow);

        const POINT    downStart{start.x, end.y + 1};
        const POINT    downEnd{end.x, end.y + 1};
        const XMFLOAT2 downStartVector = {static_cast<float>(downStart.x), static_cast<float>(downStart.y)};
        const XMFLOAT2 downEndVector   = {static_cast<float>(downEnd.x), static_cast<float>(downEnd.y)};
        UmGraphics.DebugDraw2D("Editor", XMLoadFloat2(&downStartVector), XMLoadFloat2(&downEndVector), Colors::Yellow);
    };

    DrawSubline(column, row);
}

void GridPanel::OnPlacementChange()
{
    EditablePlacementUIComponent::OnPlacementChange();

    Transform&                  transform = this->transform;
    std::vector<GridPanelSlot*> slots     = FindChildComponents<GridPanelSlot>()(transform);
    std::ranges::for_each(slots, [this](GridPanelSlot* slot) { AssignChild(*slot); });
}

void GridPanel::AssignChild(GridPanelSlot& slot) const
{
    const unsigned int columns = GetColumns();
    const unsigned int rows    = GetRows();
    slot.SetColumnsAndRows(columns, rows);

    const POINT absolutePoint = GetAbsolutePoint();
    const SIZE  size          = GetSize();
    slot.SetPlacement(absolutePoint, size);
}

void GridPanel::DrawSubline(const SublineCallback& columnSubline, const SublineCallback& rowSubline) const
{
    const unsigned int columns = Columns;
    const unsigned int rows    = Rows;
    if (columns == 0 || rows == 0)
        return;

    const auto [absoluteX, absoluteY] = GetAbsolutePoint();
    const SIZE size                   = Size;

    const long stepX = size.cx / static_cast<LONG>(columns);
    for (unsigned int i = 1; i < columns; ++i)
    {
        const POINT start{.x = absoluteX + stepX * static_cast<LONG>(i), .y = absoluteY};
        const POINT end{.x = start.x, .y = start.y + size.cy};

        columnSubline(start, end);
    }

    const long stepY = size.cy / static_cast<LONG>(rows);
    for (unsigned int i = 1; i < rows; ++i)
    {
        const POINT start{.x = absoluteX, .y = absoluteY + stepY * static_cast<LONG>(i)};
        const POINT end{.x = start.x + size.cx, .y = start.y};

        rowSubline(start, end);
    }
}

GridPanelSlot::GridPanelSlot() = default;

void GridPanelSlot::OnSetPlacement()
{
    /////////////////
    const unsigned int columns = GetColumns();
    const unsigned int rows    = GetRows();

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

    PassScopedPlacement(scopePoint, scopeSize);
}

unsigned int GridPanelSlot::GetColumns() const
{
    return ReflectFields->Columns;
}

unsigned int GridPanelSlot::GetRows() const
{
    return ReflectFields->Rows;
}

unsigned int GridPanelSlot::GetColumn() const
{
    return ReflectFields->Column;
}

unsigned int GridPanelSlot::GetRow() const
{
    return ReflectFields->Row;
}

POINT GridPanelSlot::GetCellPoint() const
{
    //const auto [pointX, pointY] = ReflectFields->Basefields.get().Basefields.get().Point;
    //const auto [sizeX, sizeY]   = ReflectFields->Basefields.get().Basefields.get().Size;
    //const long stepX            = sizeX / static_cast<LONG>(columns);
    //const long stepY            = sizeY / static_cast<LONG>(rows);

    //const POINT scopePoint{.x = pointX + stepX * static_cast<LONG>(ReflectFields->Column),
    //                       .y = pointY + stepY * static_cast<LONG>(ReflectFields->Row)};
    //const SIZE  scopeSize{.cx = stepX * static_cast<LONG>(ReflectFields->ColumnSpan),
    //                      .cy = stepY * static_cast<LONG>(ReflectFields->RowSpan)};
    return POINT{};
}

SIZE GridPanelSlot::GetCellSize() const
{
    return SIZE{};
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