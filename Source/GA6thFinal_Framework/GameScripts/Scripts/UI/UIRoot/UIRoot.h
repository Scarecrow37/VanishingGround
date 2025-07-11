#pragma once
#include "../Base/UIComponent.h"

class UIRoot : public UIComponent
{
    USING_PROPERTY(UIRoot)

public:
    UIRoot();

protected:
    REFLECT_FIELDS_BEGIN(UIComponent)

    REFLECT_FIELDS_END(UIRoot)

private:
    bool _isShowEditor;
};