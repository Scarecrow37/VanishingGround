#pragma once
#include "UI/Base/SlotComponent/SlotComponent.h"

class GridPanelSlot;

class GridPanel : public UIComponent
{
    friend GridPanelSlot;
    USING_PROPERTY(GridPanel)

public:
    static constexpr unsigned int MIN_COLUMNS = 1;
    static constexpr unsigned int MIN_ROWS    = 1;
    static constexpr unsigned int MAX_COLUMNS = 64;
    static constexpr unsigned int MAX_ROWS    = 64;

public:
    GridPanel();

public:
    REFLECT_PROPERTY(Columns, Rows)

    GETTER(unsigned int, Columns) { return ReflectFields->Columns; }
    SETTER(unsigned int, Columns)
    {
        ReflectFields->Columns = std::clamp(value, MIN_COLUMNS, MAX_COLUMNS);
        PassRowsAndColumns();
        InvalidateMeasure();
    }
    PROPERTY(Columns)

    GETTER(unsigned int, Rows) { return ReflectFields->Rows; }
    SETTER(unsigned int, Rows)
    {
        ReflectFields->Rows = std::clamp(value, MIN_ROWS, MAX_ROWS);
        PassRowsAndColumns();
        InvalidateMeasure();
    }
    PROPERTY(Rows)

    GETTER_ONLY(std::vector<GridPanelSlot*>, Slots)
    {
        std::vector<GridPanelSlot*> slots;
        Transform&                  transform = this->transform;
        for (int i = 0; i < transform.GetChildCount(); ++i)
        {
            const Transform*            child      = transform.GetChild(i);
            GameObject&                 gameObject = child->gameObject;
            std::vector<GridPanelSlot*> childSlots = GetSlots(gameObject);
            std::ranges::move(childSlots, std::back_inserter(slots));
        }
        return slots;
    }
    PROPERTY(Slots)

protected:
    void OnAttachChild(GameObject* childGameObject) override;

    void OnDrawDebugOverride() override;
    void OnDrawDebugSelectedOverride() override;

    SIZE MeasureOverride(SIZE availableSize) override;
    SIZE ArrangeOverride(SIZE finalSize) override;

private:
    void                               DrawSubline(int thickness, FXMVECTOR color) const;
    void                               PassRowsAndColumns() const;
    static std::vector<GridPanelSlot*> GetSlots(const GameObject& parentGameObject);

protected:
    REFLECT_FIELDS_BEGIN(UIComponent)
    unsigned int Columns = MIN_COLUMNS;
    unsigned int Rows    = MIN_ROWS;
    REFLECT_FIELDS_END(GridPanel)
};

class GridPanelSlot : public SlotComponent
{
    friend GridPanel;
    USING_PROPERTY(GridPanelSlot)

public:
    static constexpr unsigned int MIN_COLUMN      = 0;
    static constexpr unsigned int MIN_ROW         = 0;
    static constexpr unsigned int MIN_COLUMN_SPAN = 1;
    static constexpr unsigned int MIN_ROW_SPAN    = 1;

public:
    GridPanelSlot();

public:
    REFLECT_PROPERTY(Column, Row, ColumnSpan, RowSpan)

    GETTER(unsigned int, Column) { return ReflectFields->Column; }
    SETTER(unsigned int, Column)
    {
        ReflectFields->Column = std::clamp(value, MIN_COLUMN, ReflectFields->Columns - 1);
        ColumnSpan            = ReflectFields->ColumnSpan;
        if (GridPanel* grid = Grid; nullptr != grid)
        {
            grid->InvalidateArrange();
        }
    }
    PROPERTY(Column)

    GETTER(unsigned int, Row) { return ReflectFields->Row; }
    SETTER(unsigned int, Row)
    {
        ReflectFields->Row = std::clamp(value, MIN_ROW, ReflectFields->Rows - 1);
        RowSpan            = ReflectFields->RowSpan;
        if (GridPanel* grid = Grid; nullptr != grid)
        {
            grid->InvalidateArrange();
        }
    }
    PROPERTY(Row)

    GETTER(unsigned int, ColumnSpan) { return ReflectFields->ColumnSpan; }
    SETTER(unsigned int, ColumnSpan)
    {
        ReflectFields->ColumnSpan = std::clamp(value, MIN_COLUMN_SPAN, ReflectFields->Columns - ReflectFields->Column);
        if (UIComponent* ui = UI; nullptr != ui)
        {
            ui->InvalidateMeasure();
        }
    }
    PROPERTY(ColumnSpan)

    GETTER(unsigned int, RowSpan) { return ReflectFields->RowSpan; }
    SETTER(unsigned int, RowSpan)
    {
        ReflectFields->RowSpan = std::clamp(value, MIN_ROW_SPAN, ReflectFields->Rows - ReflectFields->Row);
        if (UIComponent* ui = UI; nullptr != ui)
        {
            ui->InvalidateMeasure();
        }
    }
    PROPERTY(RowSpan)

    GETTER_ONLY(GridPanel*, Grid)
    {
        GridPanel* grid = nullptr;
        if (const Transform* parentTransform = transform->Parent; nullptr != parentTransform)
        {
            const GameObject& parentGameObject = parentTransform->gameObject;
            grid                               = parentGameObject.GetComponent<GridPanel>();
        }
        return grid;
    }
    PROPERTY(Grid)

private:
    void SetColumnsAndRows(unsigned int columns, unsigned int rows);

protected:
    REFLECT_FIELDS_BEGIN(SlotComponent)
    unsigned int Columns;
    unsigned int Rows;
    unsigned int Column     = MIN_COLUMN;
    unsigned int Row        = MIN_ROW;
    unsigned int ColumnSpan = MIN_COLUMN_SPAN;
    unsigned int RowSpan    = MIN_ROW_SPAN;
    REFLECT_FIELDS_END(GridPanelSlot)
};



