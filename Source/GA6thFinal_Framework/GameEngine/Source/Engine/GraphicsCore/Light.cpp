#include "pch.h"
#include "Light.h"

Light::Light()
    : _data({})
    , _type(TYPE::DIRECTIONAL)
    , _isActive(true)
{
    _data.Color = {1.f, 1.f, 1.f};
    _data.Intensity = 1.f;
}

Light::~Light() {}

void Light::SetPointLight(const Vector3& color, const Vector3& position, const Vector3& attenuation, float range,
                          float intensity)
{
    _type           = TYPE::POINT;
    _data.Color     = color * range;
    _data.Intensity = intensity;

    _data.float3_1 = position;
    _data.float3_2 = attenuation;
    _data.float_1  = range;
}

void Light::SetDirectionalLight(const Vector3& color, const Vector3& ambient, const Vector3& direction, float intensity)
{
    _type           = TYPE::DIRECTIONAL;
    _data.Color     = color;
    _data.Intensity = intensity;

    _data.float3_1 = direction;
    _data.float3_2 = ambient;
}

void Light::SetSpotLight(const Vector3& color, const Vector3& position, const Vector3& direction,
                         const Vector3& attenuation, float range, float inner, float outer, float intensity)
{
    _type           = TYPE::SPOT;
    _data.Color     = color;
    _data.Intensity = intensity;

    _data.float3_1 = position;
    _data.float3_2 = direction;
    _data.float3_3 = attenuation;
    _data.float_1  = range;
    _data.float_2  = inner;
    _data.float_3  = outer;
}

void Light::SetColor(const Vector3& color)
{
    _data.Color = color;
}

void Light::SetPosition(const Vector3& position)
{
    if (_type == TYPE::DIRECTIONAL)
        return;

    _data.float3_1 = position;
}

void Light::SetDirection(const Vector3& direction)
{
    switch (_type)
    {
    case Light::TYPE::DIRECTIONAL:
        _data.float3_1 = direction;
        break;
    case Light::TYPE::SPOT:
        _data.float3_2 = direction;
        break;
    }
}

void Light::SetIntensity(float intensity)
{
    _data.Intensity = intensity;
}

void Light::SetRange(float range)
{
    if (TYPE::DIRECTIONAL == _type)
        return;

    _data.float_1 = range;
}

void Light::SetCone(float inner, float outer)
{
    if (TYPE::SPOT != _type)
        return;

    _data.float_2 = inner;
    _data.float_3 = outer;
}

void Light::SetAttenuation(float constant, float linear, float quadratic)
{
    switch (_type)
    {
    case Light::TYPE::POINT:
        _data.float3_2 = {constant, linear, quadratic};
        break;
    case Light::TYPE::SPOT:
        _data.float3_3 = {constant, linear, quadratic};
        break;
    }
}