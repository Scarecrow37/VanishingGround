#pragma once
#include "Base/LightComponent.h"

class SpotLight : public LightComponent
{
    USING_PROPERTY(SpotLight)
public:
    REFLECT_PROPERTY()

public:
    SpotLight();
    virtual ~SpotLight();

protected:
    REFLECT_FIELDS_BEGIN(LightComponent)
    REFLECT_FIELDS_END(SpotLight)
};
