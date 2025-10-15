#include "pch.h"
#include "EngineShadowPointLight.h"

void EngineShadowPointLight::SetShadowPointLight(const Vector3& color, const Vector3& position,
                                                 const Vector3& attenuation, const float& range, const float& intensity)
{
    _type      = Type::SHADOWPOINT;
    _color     = &color;
    _intensity = &intensity;
    _float3_1  = &position;
    _float3_2  = &attenuation;
    _float_1   = &range;
}
