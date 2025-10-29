#pragma once

class TutorialTestComponent : public Component
{
    USING_PROPERTY(TutorialTestComponent)

public:
    TutorialTestComponent();

public:
    REFLECT_PROPERTY()

protected:
    void ImGuiDrawPropertysEvent() override;

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(TutorialTestComponent);
};