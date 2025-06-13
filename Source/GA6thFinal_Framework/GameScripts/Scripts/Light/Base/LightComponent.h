#pragma once
#include "UmFramework.h"
class Light;

class LightComponent : public Component
{
    USING_PROPERTY(LightComponent)
public:
    REFLECT_PROPERTY(
        ReflectFields->Color,
        ReflectFields->Intensity)

public:
    LightComponent();
    virtual ~LightComponent() override;

protected:
    std::shared_ptr<Light> _light;
    REFLECT_FIELDS_BEGIN(Component)
    float Intensity = 1.f;
    std::array<float, 3> Color{1.f,1.f,1.f};    
    REFLECT_FIELDS_END(LightComponent)
};