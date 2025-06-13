#pragma once
#include "Base/LightComponent.h"
class PointLight : public LightComponent
{
    USING_PROPERTY(PointLight)
public:
    REFLECT_PROPERTY(
    Constant, 
    Linear, 
    Quadratic,
    Range
    )

public:
    PointLight();
    virtual ~PointLight();

    GETTER(float, Constant)
    {
        return ReflectFields->Constant;
    }
    SETTER(float, Constant)
    {
        ReflectFields->Constant = value;
        Lighting.SetAttenuation(ReflectFields->Constant, ReflectFields->Linear, ReflectFields->Quadratic);
    }
    PROPERTY(Constant)

    GETTER(float, Linear) 
    { 
        return ReflectFields->Linear; 
    }
    SETTER(float, Linear)
    {
        ReflectFields->Linear = value;
        Lighting.SetAttenuation(ReflectFields->Constant, ReflectFields->Linear, ReflectFields->Quadratic);
    }
    PROPERTY(Linear)

    GETTER(float, Quadratic) 
    { 
        return ReflectFields->Quadratic; 
    }
    SETTER(float, Quadratic)
    {
        ReflectFields->Quadratic = value;
        Lighting.SetAttenuation(ReflectFields->Constant, ReflectFields->Linear, ReflectFields->Quadratic);
    }
    PROPERTY(Quadratic)

    GETTER(float, Range)
    { 
        return ReflectFields->Range; 
    }
    SETTER(float, Range)
    { 
        ReflectFields->Range = value;
        Lighting.SetRange(value);
    }
    PROPERTY(Range)

protected:
    REFLECT_FIELDS_BEGIN(LightComponent)
    float Constant = 1.f;
    float Linear = 0.1f;
    float Quadratic = 0.1f;
    float Range = 1.f;
    REFLECT_FIELDS_END(PointLight)
};
