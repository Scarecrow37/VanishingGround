#pragma once
#include "UI/Base/EditablePlacementUIComponent/EditablePlacementUIComponent.h"
#include "UI/Base/PanelSlotComponent/PanelSlotComponent.h"

class AnchorPanelSlot;

class AnchorPanel : public EditablePlacementUIComponent
{
    USING_PROPERTY(AnchorPanel)
public:
    AnchorPanel();

public:
    void OnAttachChild(GameObject* childGameObject) override;

protected:
    void OnPlacementChange() override;

private:
    void AssignChild(AnchorPanelSlot& slot) const;

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

public:
    REFLECT_PROPERTY(Pivot)

    GETTER(Vector2, Pivot)
    {
        return Vector2{ReflectFields->PivotX, ReflectFields->PivotY};
    }
    SETTER(Vector2, Pivot)
    {
        ReflectFields->PivotX = std::clamp(value.x, 0.0f, 1.0f);
        ReflectFields->PivotY = std::clamp(value.y, 0.0f, 1.0f);
        OnPlacementChange();
    }
    PROPERTY(Pivot)

public:
    AnchorType GetAnchorType() const;
    POINT      GetAnchorPoint() const;
    POINT      GetOffsetPoint() const;
    SIZE       GetSiblingSize() const;

protected:
    void OnPlacementChange() override;
    void ImGuiDrawPropertysEvent() override;
    void DrawDebugSelected() override;

private:
    void UpdateAnchorPoint();
    void UpdateOffsetPoint();

protected:
    REFLECT_FIELDS_BEGIN(PanelSlotComponent)
    AnchorType Anchor = AnchorType::TOP_LEFT;
    POINT      AnchorPoint;
    float      PivotX = 0.0f;
    float      PivotY = 0.0f;
    POINT      OffsetPoint;
    SIZE       SiblingSize;
    REFLECT_FIELDS_END(AnchorPanelSlot)

};