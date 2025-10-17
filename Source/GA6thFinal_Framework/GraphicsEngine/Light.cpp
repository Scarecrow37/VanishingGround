#include "pch.h"
#include "Light.h"

Light::Light()
{
    _data.Color     = {1.f, 1.f, 1.f};
    _data.Intensity = 1.f;
}

Light::~Light() = default;

bool Light::IsActive() const
{
    return GraphicsBase::IsActive();
}

void Light::SetActive(const bool* isActive)
{
    GraphicsBase::SetActive(isActive);
}

void Light::SetDirectionalLight(const Vector3& color, const Vector3& ambient, const Vector3& direction, const float& intensity)
{
    _type      = Type::DIRECTIONAL;
    _color     = &color;
    _intensity = &intensity;
    _float3_1  = &direction;
    _float3_2  = &ambient;
}

void Light::SetPointLight(const Vector3& color, const Vector3& position, const Vector3& attenuation, const float& range, const float& intensity)
{
    _type      = Type::POINT;
    _color     = &color;
    _intensity = &intensity;
    _float3_1  = &position;
    _float3_2  = &attenuation;
    _float_1   = &range;
}

void Light::SetSpotLight(const Vector3& color, const Vector3& position, const Vector3& direction,
                         const Vector3& attenuation, const float& range, const float& inner, const float& outer,
                         const float& intensity)
{
    _type      = Type::SPOT;
    _color     = &color;
    _intensity = &intensity;
    _float3_1  = &position;
    _float3_2  = &direction;
    _float3_3  = &attenuation;
    _float_1   = &range;
    _float_2   = &inner;
    _float_3   = &outer;
}

void Light::SetShadowPointLight(const Vector3& color, const Vector3& position, const Vector3& attenuation,
                                const float& range, const float& intensity)
{
    _type      = Type::SHADOWPOINT;
    _color     = &color;
    _intensity = &intensity;
    _float3_1  = &position;
    _float3_2  = &attenuation;
    _float_1   = &range;
}


void Light::AddReference()
{
    GraphicsBase::AddReference();
}

void Light::Release()
{
    GraphicsBase::Release();
}

void Light::Update(const float deltaTime)
{
    if (_type == Type::NONE)
        return;

    _data.Color = *_color;
    _data.Intensity = *_intensity;

    switch (_type)
    {
    case Light::Type::DIRECTIONAL:
        _data.float3_1 = *_float3_1;
        _data.float3_2 = *_float3_2;
        break;
    case Light::Type::POINT:
        _data.float3_1 = *_float3_1;
        _data.float3_2 = *_float3_2;
        _data.float_1  = *_float_1;
        break;
    case Light::Type::SPOT:
        _data.float3_1 = *_float3_1;
        _data.float3_2 = *_float3_2;
        _data.float3_3 = *_float3_3;
        _data.float_1  = *_float_1;
        //_data.float_2  = *_float_2;
        //_data.float_3  = *_float_3;
        _data.float_2  = cosf(XMConvertToRadians(*_float_2));
        _data.float_3  = cosf(XMConvertToRadians(*_float_3));
        break;
    case Light::Type::SHADOWPOINT:
        _data.float3_1 = *_float3_1;
        _data.float3_2 = *_float3_2;
        _data.float_1  = *_float_1;
        break;
    }       
}