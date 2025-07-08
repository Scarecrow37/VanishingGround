#pragma once
#include "UmFramework.h"
class TestParticle : public Component
{
    USING_PROPERTY(TestParticle)
public:
    REFLECT_PROPERTY()

public:
    TestParticle();
    virtual ~TestParticle();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(TestParticle)
};
