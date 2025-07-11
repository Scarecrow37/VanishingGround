#pragma once
#include "UI/Base/AreaUIComponent/AreaUIComponent.h"
#include "UI/Base/PanelSlotComponent/PanelSlotComponent.h"

class GridPanel : public AreaUIComponent
{
    USING_PROPERTY(GridPanel)
public:
    GridPanel();

    REFLECT_PROPERTY(Columns, Rows)

    GETTER(unsigned int, Columns) { return ReflectFields->Columns; }
    SETTER(unsigned int, Columns) { ReflectFields->Columns = std::clamp(value, 1u, 64u); }
    PROPERTY(Columns)

    GETTER(unsigned int, Rows) { return ReflectFields->Rows; }
    SETTER(unsigned int, Rows) { ReflectFields->Rows = std::clamp(value, 1u, 64u); }
    PROPERTY(Rows)

protected:
    REFLECT_FIELDS_BEGIN(AreaUIComponent)
    unsigned int Columns = 1;
    unsigned int Rows    = 1;
    REFLECT_FIELDS_END(GridPanel)

    void OnAttachChild(GameObject* childGameObject) override;

    void OnDrawDebug() override;

    void OnDrawDebugSelected() override;

    void Reset() override;

private:
    void DrawLine(FXMVECTOR color) const;
};

class GridPanelSlot : public PanelSlotComponent
{
    friend GridPanel;
    USING_PROPERTY(GridPanelSlot)
public:
    GridPanelSlot();

    REFLECT_PROPERTY(Column, Row)

    GETTER(unsigned int, Column) { return ReflectFields->Column; }
    SETTER(unsigned int, Column)
    {
        if (nullptr == _gridPanel)
            return;
        const unsigned int columns = _gridPanel->Columns;
        ReflectFields->Column      = std::clamp(value, 0u, columns - 1);
    }
    PROPERTY(Column)

    GETTER(unsigned int, Row) { return ReflectFields->Row; }
    SETTER(unsigned int, Row)
    {
        if (nullptr == _gridPanel)
            return;
        const unsigned int rows = _gridPanel->Rows;
        ReflectFields->Row      = std::clamp(value, 0u, rows - 1);
    }
    PROPERTY(Row)

protected:
    REFLECT_FIELDS_BEGIN(PanelSlotComponent)
    unsigned int Column = 0;
    unsigned int Row    = 0;
    REFLECT_FIELDS_END(GridPanelSlot)

private:
    void SetGridPanel(GridPanel* gridPanel);

    GridPanel* _gridPanel;
};