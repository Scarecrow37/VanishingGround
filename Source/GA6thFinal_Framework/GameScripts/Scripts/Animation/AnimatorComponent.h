#pragma once
#include "UmFramework.h"

class AnimatorComponent : public Component
{
    USING_PROPERTY(AnimatorComponent)
public:
    REFLECT_PROPERTY()

public:
    AnimatorComponent();
    virtual ~AnimatorComponent();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(AnimatorComponent)
};
