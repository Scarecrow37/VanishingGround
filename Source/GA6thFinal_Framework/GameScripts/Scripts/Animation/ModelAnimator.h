#pragma once
#include "UmFramework.h"

class ModelAnimator : public Component
{
    USING_PROPERTY(ModelAnimator)
public:
    REFLECT_PROPERTY()

public:
    ModelAnimator();
    virtual ~ModelAnimator();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(ModelAnimator)
};
