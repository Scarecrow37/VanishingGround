#pragma once
#include "UmFramework.h"
class GridPanel : public Component
{
    USING_PROPERTY(GridPanel)
public:
    GridPanel() = default;
    ~GridPanel() override = default;

    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(GridPanel)
};
