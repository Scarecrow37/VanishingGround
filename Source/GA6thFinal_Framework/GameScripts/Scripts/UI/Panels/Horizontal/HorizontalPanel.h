#pragma once
#include "UI/Base/SlotComponent/SlotComponent.h"

class HorizontalPanelSlot;

class HorizontalPanel : public UIComponent
{
    friend HorizontalPanelSlot;
    USING_PROPERTY(HorizontalPanel)

public:
    HorizontalPanel();

public:
    REFLECT_PROPERTY(Space, LineSpace)

    GETTER_ONLY(std::vector<HorizontalPanelSlot*>, Slots)
    {
        std::vector<HorizontalPanelSlot*> slots;
        Transform&                        transform = this->transform;
        for (int i = 0; i < transform.GetChildCount(); ++i)
        {
            const Transform*            child      = transform.GetChild(i);
            GameObject&                 gameObject = child->gameObject;
            std::vector<HorizontalPanelSlot*> childSlots = GetSlots(gameObject);
            std::ranges::move(childSlots, std::back_inserter(slots));
        }
        return slots;
    }
    PROPERTY(Slots)

    GETTER(int, Space) { return ReflectFields->Space; }
    SETTER(int, Space)
    {
        ReflectFields->Space = static_cast<LONG>(value);
        InvalidateMeasure();
        InvalidateArrange();
    }
    PROPERTY(Space)

    GETTER(int, LineSpace) { return ReflectFields->LineSpace; }
    SETTER(int, LineSpace)
    {
        ReflectFields->LineSpace = static_cast<LONG>(value);
        InvalidateMeasure();
        InvalidateArrange();
    }
    PROPERTY(LineSpace)

protected:
    void OnAttachChild(GameObject* childGameObject) override;

    SIZE MeasureOverride(const SIZE availableSize) override;
    SIZE ArrangeOverride(const SIZE finalSize) override;

private:
    static std::vector<HorizontalPanelSlot*> GetSlots(const GameObject& parentGameObject);

protected:
    REFLECT_FIELDS_BEGIN(UIComponent)
    LONG Space     = 0;
    LONG LineSpace = 0;
    REFLECT_FIELDS_END(HorizontalPanel)

};

class HorizontalPanelSlot : public SlotComponent
{
    friend HorizontalPanel;
    USING_PROPERTY(HorizontalPanelSlot)

public:
    REFLECT_PROPERTY(IsStretch)

    GETTER(bool, IsStretch) { return ReflectFields->IsStretch; }
    SETTER(bool, IsStretch)
    {
        ReflectFields->IsStretch = value;
        if (UIComponent* ui = UI; nullptr != ui)
        {
            ui->InvalidateMeasure();
        }
    }
    PROPERTY(IsStretch)

    GETTER_ONLY(HorizontalPanel*, Horizontal)
    {
        HorizontalPanel* horizontal = nullptr;
        if (const Transform* parentTransform = transform->Parent; nullptr != parentTransform)
        {
            const GameObject& parentGameObject = parentTransform->gameObject;
            horizontal                         = parentGameObject.GetComponent<HorizontalPanel>();
        }
        return horizontal;
    }
    PROPERTY(Horizontal)

    GETTER_ONLY(LONG, Line) { return ReflectFields->Line; }
    PROPERTY(Line)

public:
    HorizontalPanelSlot();

protected:
    void ImGuiDrawPropertysEvent() override;

private:
    void SetLine(LONG line);

protected:
    REFLECT_FIELDS_BEGIN(SlotComponent)
    bool IsStretch = false;
    LONG Line      = 0;
    REFLECT_FIELDS_END(HorizontalPanelSlot)
};