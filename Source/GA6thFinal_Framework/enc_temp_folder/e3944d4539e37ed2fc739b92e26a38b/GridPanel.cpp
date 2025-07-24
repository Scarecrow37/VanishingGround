#include "pchScripts.h"
#include "GridPanel.h"

GridPanel::GridPanel() = default;

void GridPanel::OnAttachChild(GameObject* childGameObject)
{
    EditablePlacementUIComponent::OnAttachChild(childGameObject);
    auto& slot = childGameObject->AddComponent<GridPanelSlot>();
    slot.SetColumnsAndRows(ReflectFields->Columns, ReflectFields->Rows);
    POINT point = ReflectFields->Basefields.get().Basefields.get().Point;
    auto [x, y] = ReflectFields->Basefields.get().Basefields.get().ScopePoint;
    point.x += x;
    point.y += y;
    slot.SetPlacement(point, ReflectFields->Basefields.get().Basefields.get().Size);
}

void GridPanel::DrawDebug()
{
    EditablePlacementUIComponent::DrawDebug();
    const unsigned int columns = Columns;
    const unsigned int rows    = Rows;
    const SIZE         size    = Size;
    const POINT        point   = Point;
    const auto         [scopeX, scopeY]  = ReflectFields->Basefields.get().Basefields.get().ScopePoint;
    if (columns == 0 || rows == 0)
        return;

    const long stepX = size.cx / static_cast<LONG>(columns);
    for (unsigned int i = 1; i < columns; ++i)
    {
        const POINT start{
            .x = scopeX + point.x + stepX * static_cast<LONG>(i),
            .y = scopeY + point.y
        };

        const POINT end{
            .x = start.x,
            .y = start.y + size.cy
        };

        XMFLOAT2 startVector = {static_cast<float>(start.x), static_cast<float>(start.y)};
        XMFLOAT2 endVector   = {static_cast<float>(end.x), static_cast<float>(end.y)};
        UmGraphics.DebugDraw2D("Editor", XMLoadFloat2(&startVector), XMLoadFloat2(&endVector), DirectX::Colors::White);
    }

    const long stepY = size.cy / static_cast<LONG>(rows);
    for (unsigned int i = 1; i < rows; ++i)
    {
        const POINT start{
            .x = scopeX + point.x,
            .y = scopeY + point.y + stepY * static_cast<LONG>(i)
        };
        const POINT end{
            .x = start.x + size.cx,
            .y = start.y
        };

         XMFLOAT2 startVector = {static_cast<float>(start.x), static_cast<float>(start.y)};
        XMFLOAT2 endVector   = {static_cast<float>(end.x), static_cast<float>(end.y)};
        UmGraphics.DebugDraw2D("Editor", XMLoadFloat2(&startVector), XMLoadFloat2(&endVector), DirectX::Colors::White);
    }
}

void GridPanel::DrawDebugSelected()
{
    EditablePlacementUIComponent::DrawDebugSelected();
    const unsigned int columns = Columns;
    const unsigned int rows    = Rows;
    const SIZE         size    = Size;
    const POINT        point   = Point;
    const auto [scopeX, scopeY] = ReflectFields->Basefields.get().Basefields.get().ScopePoint;
    if (columns == 0 || rows == 0)
        return;

    const long stepX = size.cx / static_cast<LONG>(columns);
    for (unsigned int i = 1; i < columns; ++i)
    {
        const POINT start{.x = scopeX + point.x + stepX * static_cast<LONG>(i), .y = scopeY + point.y};

        const POINT end{.x = start.x, .y = start.y + size.cy};


        XMFLOAT2 startVector = {static_cast<float>(start.x), static_cast<float>(start.y)};
        XMFLOAT2 endVector   = {static_cast<float>(end.x), static_cast<float>(end.y)};
        UmGraphics.DebugDraw2D("Editor", XMLoadFloat2(&startVector), XMLoadFloat2(&endVector), DirectX::Colors::Yellow);


        const POINT leftStart{start.x - 1, start.y};
        const POINT leftEnd{start.x - 1, end.y};
        XMFLOAT2 leftStartVector = {static_cast<float>(leftStart.x), static_cast<float>(leftStart.y)};
        XMFLOAT2 leftEndVector   = {static_cast<float>(leftEnd.x), static_cast<float>(leftEnd.y)};
        UmGraphics.DebugDraw2D("Editor", XMLoadFloat2(&leftStartVector), XMLoadFloat2(&leftEndVector),
                               DirectX::Colors::Yellow);

        const POINT rightStart{end.x + 1, start.y};
        const POINT rightEnd{end.x + 1, end.y};
        XMFLOAT2 rightStartVector = {static_cast<float>(rightStart.x), static_cast<float>(rightStart.y)};
        XMFLOAT2 rightEndVector   = {static_cast<float>(rightEnd.x), static_cast<float>(rightEnd.y)};
        UmGraphics.DebugDraw2D("Editor", XMLoadFloat2(&rightStartVector), XMLoadFloat2(&rightEndVector),
                               DirectX::Colors::Yellow);
    }

    const long stepY = size.cy / static_cast<LONG>(rows);
    for (unsigned int i = 1; i < rows; ++i)
    {
        const POINT start{.x = scopeX + point.x, .y = scopeY + point.y + stepY * static_cast<LONG>(i)};
        const POINT end{.x = start.x + size.cx, .y = start.y};

        XMFLOAT2 startVector = {static_cast<float>(start.x), static_cast<float>(start.y)};
        XMFLOAT2 endVector   = {static_cast<float>(end.x), static_cast<float>(end.y)};
        UmGraphics.DebugDraw2D("Editor", XMLoadFloat2(&startVector), XMLoadFloat2(&endVector), DirectX::Colors::Yellow);

        const POINT upStart{start.x, start.y - 1};
        const POINT upEnd{end.x, start.y - 1};
        XMFLOAT2 upStartVector = {static_cast<float>(upStart.x), static_cast<float>(upStart.y)};
        XMFLOAT2 upEndVector   = {static_cast<float>(upEnd.x), static_cast<float>(upEnd.y)};
        UmGraphics.DebugDraw2D("Editor", XMLoadFloat2(&upStartVector), XMLoadFloat2(&upEndVector),
                               DirectX::Colors::Yellow);

        const POINT downStart{start.x, end.y + 1};
        const POINT downEnd{end.x, end.y + 1};
        XMFLOAT2 downStartVector = {static_cast<float>(downStart.x), static_cast<float>(downStart.y)};
        XMFLOAT2 downEndVector   = {static_cast<float>(downEnd.x), static_cast<float>(downEnd.y)};
        UmGraphics.DebugDraw2D("Editor", XMLoadFloat2(&downStartVector), XMLoadFloat2(&downEndVector),
                               DirectX::Colors::Yellow);
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
                slot->SetColumnsAndRows(ReflectFields->Columns, ReflectFields->Rows);
                POINT point  = ReflectFields->Basefields.get().Basefields.get().Point;
                auto  [x, y] = ReflectFields->Basefields.get().Basefields.get().ScopePoint;
                point.x += x;
                point.y += y;
                slot->SetPlacement(point,
                                  ReflectFields->Basefields.get().Basefields.get().Size);
            }
        }
    }
}

GridPanelSlot::GridPanelSlot() = default;

void GridPanelSlot::OnSetPlacement()
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

    PassScopedPlacement(scopePoint, scopeSize);
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