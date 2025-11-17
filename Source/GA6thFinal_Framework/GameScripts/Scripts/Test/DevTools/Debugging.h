#pragma once

class Debugging : public Component
{
    USING_PROPERTY(Debugging)
public:
    REFLECT_PROPERTY()

public:
    void Update() override;

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(Debugging)

};
