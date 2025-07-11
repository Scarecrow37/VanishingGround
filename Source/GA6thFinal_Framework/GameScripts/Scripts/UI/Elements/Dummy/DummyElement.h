#pragma once
#include "UI/Base/AreaUIComponent/AreaUIComponent.h"

class DummyElement : public AreaUIComponent
{
    USING_PROPERTY(DummyElement)
public:
    DummyElement();

protected:
    REFLECT_FIELDS_BEGIN(AreaUIComponent)
    REFLECT_FIELDS_END(DummyElement)
};