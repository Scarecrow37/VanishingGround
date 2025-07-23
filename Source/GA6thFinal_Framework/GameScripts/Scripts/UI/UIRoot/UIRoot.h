#pragma once
#include "UI/Base/PlacementUIComponent/PlacementUIComponent.h"
#include "UI/Base/PanelSlotComponent/PanelSlotComponent.h"

class UIRoot : public PlacementUIComponent
{
    USING_PROPERTY(UIRoot)

public:
    UIRoot();

protected:
    REFLECT_FIELDS_BEGIN(PlacementUIComponent)
    REFLECT_FIELDS_END(UIRoot)

    void OnAttachChild(GameObject* childGameObject) override;
    void OnDetachParent(GameObject* previousParentGameObject) override;
    void ImGuiDrawPropertysEvent() override;
    void OnPlacementChange() override;

private:
    void GetSizeFromViewport();
};

class UIRootSlot : public PanelSlotComponent
{
    friend UIRoot;
    USING_PROPERTY(UIRootSlot)

public:
    UIRootSlot();

    void OnSetPlacement() override;
};