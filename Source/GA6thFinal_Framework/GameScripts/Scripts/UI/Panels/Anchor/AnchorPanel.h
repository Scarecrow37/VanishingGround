#pragma once
#include "UI/Base/EditablePlacementUIComponent/EditablePlacementUIComponent.h"
#include "UI/Base/PanelSlotComponent/PanelSlotComponent.h"

class AnchorPanel : public EditablePlacementUIComponent
{
    USING_PROPERTY(AnchorPanel)
public:
    AnchorPanel();

    void OnAttachChild(GameObject* childGameObject) override;
    void OnPlacementChange() override;

protected:
    REFLECT_FIELDS_BEGIN(EditablePlacementUIComponent)
    REFLECT_FIELDS_END(AnchorPanel)
};

class AnchorPanelSlot : public PanelSlotComponent
{
    friend AnchorPanel;
    USING_PROPERTY(AnchorPanelSlot)

public:
    enum class AnchorType : unsigned char
    {
        TOP_LEFT,
        TOP_CENTER,
        TOP_RIGHT,
        CENTER_LEFT,
        CENTER,
        CENTER_RIGHT,
        BOTTOM_LEFT,
        BOTTOM_CENTER,
        BOTTOM_RIGHT
    };

public:
    AnchorPanelSlot();

    void OnSetPlacement() override;

protected:
    void ImGuiDrawPropertysEvent() override;
    void DrawDebugSelected() override;

protected:
    REFLECT_FIELDS_BEGIN(PanelSlotComponent)
    AnchorType Anchor = AnchorType::TOP_LEFT;
    POINT      AnchorPoint;
    REFLECT_FIELDS_END(AnchorPanelSlot)
};