#pragma once
class AbandonYesButton : public Component
{
    USING_PROPERTY(AbandonYesButton)

public:
    AbandonYesButton();
    ~AbandonYesButton() override;

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(AbandonYesButton)
};

