#pragma once
#include "UmFramework.h"
class AccessorySystem : public Component
{
    USING_PROPERTY(AccessorySystem)

public:
    AccessorySystem();
    ~AccessorySystem() override;

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(AccessorySystem)
};
