#pragma once

class SelectPopup : public Component
{
    USING_PROPERTY(SelectPopup)

public:
    SelectPopup();
    ~SelectPopup() override;

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(SelectPopup)
};

