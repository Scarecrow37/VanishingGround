#pragma once
#include "Base/LightComponent.h"
class PointLight : public LightComponent
{
    USING_PROPERTY(PointLight)
public:
    REFLECT_PROPERTY(
    ReflectFields->Constant, 
    ReflectFields->Linear, 
    ReflectFields->Quadratic,
    ReflectFields->Range
    )

public:
    PointLight();
    virtual ~PointLight();

protected:
    REFLECT_FIELDS_BEGIN(LightComponent)
    float Constant = 1.f;
    float Linear = 0.1f;
    float Quadratic = 0.1f;
    float Range = 1.f;
    REFLECT_FIELDS_END(PointLight)

    virtual void FixedUpdate() override;
};
