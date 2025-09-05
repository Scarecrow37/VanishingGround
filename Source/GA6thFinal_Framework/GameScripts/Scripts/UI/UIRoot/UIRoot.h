#pragma once

class UIRootSlot;

class UIRoot : public UIBaseComponent
{
    USING_PROPERTY(UIRoot)

public:
    UIRoot();

public:
    void SortViewOrder() const;

protected:
    REFLECT_FIELDS_BEGIN(UIBaseComponent)
    REFLECT_FIELDS_END(UIRoot)
};