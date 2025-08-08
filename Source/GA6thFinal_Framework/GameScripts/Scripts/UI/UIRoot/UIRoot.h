#pragma once
#include "UI/Base/PlacementUIComponent/PlacementUIComponent.h"
#include "UI/Base/PanelSlotComponent/PanelSlotComponent.h"

class UIRootSlot;

class UIRoot : public PlacementUIComponent
{
    USING_PROPERTY(UIRoot)

public:
    UIRoot();

public:
    void SortViewOrder() const;

protected:
    void OnAttachChild(GameObject* childGameObject) override;
    void OnDetachParent(GameObject* previousParentGameObject) override;
    void ImGuiDrawPropertysEvent() override;
    void OnPlacementChange() override;

private:
    void AssignChild(UIRootSlot& slot) const;
    void GetSizeFromViewport();

protected:
    REFLECT_FIELDS_BEGIN(PlacementUIComponent)
    REFLECT_FIELDS_END(UIRoot)

};

class UIRootSlot : public PanelSlotComponent
{
    friend UIRoot;
    USING_PROPERTY(UIRootSlot)

public:
    UIRootSlot();

protected:
    void OnPlacementChange() override;
};