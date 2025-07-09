#pragma once
#include "UI/Base/UIComponent.h"

class ButtonElement : public UIComponent
{
    USING_PROPERTY(ButtonElement)
public:
    REFLECT_PROPERTY()

public:
    ButtonElement();
    ~ButtonElement() override;

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(ButtonElement)
};
