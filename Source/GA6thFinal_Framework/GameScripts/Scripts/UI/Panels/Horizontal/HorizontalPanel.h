#pragma once
#include "UI/Base/EditablePlacementUIComponent/EditablePlacementUIComponent.h"
#include "UI/Base/PanelSlotComponent/PanelSlotComponent.h"

class HorizontalPanelSlot;

class HorizontalPanel : public EditablePlacementUIComponent
{
    USING_PROPERTY(HorizontalPanel)

public:
    HorizontalPanel();

public:
    void OnAttachChild(GameObject* childGameObject) override;
    void OnChildPlacementChange(PlacementUIComponent* changedComponent) override;

protected:
    void OnPlacementChange() override;

private:
    void AssignChild(HorizontalPanelSlot& slot, unsigned int index, LONG offset, LONG limit) const;

protected:
    REFLECT_FIELDS_BEGIN(EditablePlacementUIComponent)
    REFLECT_FIELDS_END(HorizontalPanel)

};

class HorizontalPanelSlot : public PanelSlotComponent
{
    friend HorizontalPanel;
    USING_PROPERTY(HorizontalPanelSlot)

public:
    REFLECT_PROPERTY(Order, IsStretch)

    GETTER_ONLY(unsigned int, Order) { return ReflectFields->Order; }
    PROPERTY(Order)

    GETTER(bool, IsStretch) { return ReflectFields->IsStretch; }
    SETTER(bool, IsStretch)
    {
        ReflectFields->IsStretch = value;
        SpreadPlacementToParent();
    }
    PROPERTY(IsStretch)

protected:
    void OnPlacementChange() override;

private:
    LONG GetLimit() const;
    LONG GetOffset() const;
    LONG GetSiblingWidth() const;
    void SetLayout(unsigned int order, LONG offset, LONG limit);

protected:
    REFLECT_FIELDS_BEGIN(PanelSlotComponent)
    unsigned int Order     = 0;
    LONG         Offset    = 0;
    LONG         Limit     = 0;
    bool         IsStretch = false;
    REFLECT_FIELDS_END(HorizontalPanelSlot)
};