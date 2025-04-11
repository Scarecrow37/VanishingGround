#pragma once
#include "UmFramework.h"
class MyComponent : public Component
{
    USING_PROPERTY(MyComponent)
public:
REFLECT_PROPERTY()

public:
    MyComponent();
    virtual ~MyComponent();

    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(MyComponent)
};
