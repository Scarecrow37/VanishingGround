#pragma once
#include "UI/Base/EditablePlacementUIComponent/EditablePlacementUIComponent.h"
#include "UI/Base/PanelSlotComponent/PanelSlotComponent.h"

class GridPanel : public EditablePlacementUIComponent
{
    USING_PROPERTY(GridPanel)
public:
    static constexpr unsigned int MIN_COLUMNS = 1;
    static constexpr unsigned int MIN_ROWS    = 1;
    static constexpr unsigned int MAX_COLUMNS = 64;
    static constexpr unsigned int MAX_ROWS    = 64;

    GridPanel();

    REFLECT_PROPERTY(Columns, Rows)

    GETTER(unsigned int, Columns) { return ReflectFields->Columns; }
    SETTER(unsigned int, Columns)
    {
        ReflectFields->Columns = std::clamp(value, MIN_COLUMNS, MAX_COLUMNS);
        OnPlacementChange();
    }
    PROPERTY(Columns)

    GETTER(unsigned int, Rows) { return ReflectFields->Rows; }
    SETTER(unsigned int, Rows)
    {
        ReflectFields->Rows = std::clamp(value, MIN_ROWS, MAX_ROWS);
        OnPlacementChange();
    }
    PROPERTY(Rows)

protected:
    REFLECT_FIELDS_BEGIN(EditablePlacementUIComponent)
    unsigned int Columns = MIN_COLUMNS;
    unsigned int Rows    = MIN_ROWS;
    REFLECT_FIELDS_END(GridPanel)

    void OnAttachChild(GameObject* childGameObject) override;

    void DrawDebug() override;

    void DrawDebugSelected() override;

    void OnPlacementChange() override;
};

class GridPanelSlot : public PanelSlotComponent
{
    friend GridPanel;
    USING_PROPERTY(GridPanelSlot)

public:
    static constexpr unsigned int MIN_COLUMN      = 0;
    static constexpr unsigned int MIN_ROW         = 0;
    static constexpr unsigned int MIN_COLUMN_SPAN = 1;
    static constexpr unsigned int MIN_ROW_SPAN    = 1;

    GridPanelSlot();

public:
    REFLECT_PROPERTY(Column, Row, ColumnSpan, RowSpan)

    GETTER(unsigned int, Column) { return ReflectFields->Column; }
    SETTER(unsigned int, Column)
    {
        ReflectFields->Column = std::clamp(value, MIN_COLUMN, ReflectFields->Columns - 1);
        ColumnSpan            = ReflectFields->ColumnSpan;
    }
    PROPERTY(Column)

    GETTER(unsigned int, Row) { return ReflectFields->Row; }
    SETTER(unsigned int, Row)
    {
        ReflectFields->Row = std::clamp(value, MIN_ROW, ReflectFields->Rows - 1);
        RowSpan            = ReflectFields->RowSpan;
    }
    PROPERTY(Row)

    GETTER(unsigned int, ColumnSpan) { return ReflectFields->ColumnSpan; }
    SETTER(unsigned int, ColumnSpan)
    {
        ReflectFields->ColumnSpan = std::clamp(value, MIN_COLUMN_SPAN, ReflectFields->Columns - ReflectFields->Column);
        OnSetPlacement();
    }
    PROPERTY(ColumnSpan)

    GETTER(unsigned int, RowSpan) { return ReflectFields->RowSpan; }
    SETTER(unsigned int, RowSpan)
    {
        ReflectFields->RowSpan = std::clamp(value, MIN_ROW_SPAN, ReflectFields->Rows - ReflectFields->Row);
        OnSetPlacement();
    }
    PROPERTY(RowSpan)

public:
    void OnSetPlacement() override;

protected:
    REFLECT_FIELDS_BEGIN(PanelSlotComponent)
    unsigned int Columns    = GridPanel::MIN_COLUMNS;
    unsigned int Rows       = GridPanel::MIN_ROWS;
    unsigned int Column     = MIN_COLUMN;
    unsigned int Row        = MIN_ROW;
    unsigned int ColumnSpan = MIN_COLUMN_SPAN;
    unsigned int RowSpan    = MIN_ROW_SPAN;
    REFLECT_FIELDS_END(GridPanelSlot)

private:
    void SetColumnsAndRows(unsigned int columns, unsigned int rows);
    void SetColumns(unsigned int columns);
    void SetRows(unsigned int rows);
};