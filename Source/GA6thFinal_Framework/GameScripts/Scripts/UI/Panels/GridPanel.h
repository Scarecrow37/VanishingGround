#pragma once
#include "UI/Base/UIComponent.h"

class GridPanel : public UIComponent
{
    USING_PROPERTY(GridPanel)
public:
    GridPanel()           = default;
    ~GridPanel() override = default;

    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(GridPanel)
};

class GridPanelSlot : public UIComponent
{
    USING_PROPERTY(GridPanelSlot)
public:
    GridPanelSlot()           = default;
    ~GridPanelSlot() override = default;

    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(GridPanelSlot)
};