#pragma once
#include "UI/Base/EditablePlacementUIComponent/EditablePlacementUIComponent.h"

class DummyElement : public EditablePlacementUIComponent
{
    USING_PROPERTY(DummyElement)
public:
    DummyElement();

protected:
    REFLECT_FIELDS_BEGIN(EditablePlacementUIComponent)
    REFLECT_FIELDS_END(DummyElement)
};