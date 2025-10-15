#pragma once
#include "Light.h"

class EngineShadowPointLight : public Light
{
public:
    EngineShadowPointLight() = default;
    virtual ~EngineShadowPointLight() = default;

public:
    void SetShadowPointLight(const Vector3& color, const Vector3& position, const Vector3& attenuation,
                             const float& range, const float& intensity) override;
};
