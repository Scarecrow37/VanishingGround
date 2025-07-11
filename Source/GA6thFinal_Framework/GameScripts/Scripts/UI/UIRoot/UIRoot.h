#pragma once
#include "UI/Base/PanelSlotComponent/PanelSlotComponent.h"

class UIRoot : public UIComponent
{
    USING_PROPERTY(UIRoot)

public:
    UIRoot();

protected:
    REFLECT_FIELDS_BEGIN(UIComponent)
    REFLECT_FIELDS_END(UIRoot)

    void OnDrawDebug() override;

    void OnDrawDebugSelected() override;

    void OnAttachChild(GameObject* childGameObject) override;

private:
    SIZE _size;
};

class UIRootSlot : public PanelSlotComponent
{
    USING_PROPERTY(UIRootSlot)

public:
    UIRootSlot();

protected:
    REFLECT_FIELDS_BEGIN(PanelSlotComponent)
    REFLECT_FIELDS_END(UIRootSlot)
};