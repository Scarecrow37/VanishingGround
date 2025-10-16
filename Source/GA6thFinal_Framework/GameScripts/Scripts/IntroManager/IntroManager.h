#pragma once

class IntroManager : public Component
{
    USING_PROPERTY(IntroManager)

public:
    IntroManager();

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(IntroManager)
};