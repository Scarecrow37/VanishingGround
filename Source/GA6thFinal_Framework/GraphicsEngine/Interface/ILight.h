#pragma once
#include "IGraphicsBase.h"

class ILight : public IGraphicsBase
{
protected:
    ILight()          = default;
    virtual ~ILight() = default;

public:
    virtual void SetDirectionalLight(const Vector3& color, const Vector3& ambient, const Vector3& direction, const float& intensity) = 0;
    virtual void SetPointLight(const Vector3& color, const Vector3& position, const Vector3& attenuation, const float& range, const float& intensity) = 0;
    virtual void SetSpotLight(const Vector3& color, const Vector3& position, const Vector3& direction, const Vector3& attenuation, const float& range, const float& inner, const float& outer, const float& intensity) = 0;
    virtual void SetShadowPointLight(const Vector3& color, const Vector3& position, const Vector3& attenuation, const float& range, const float& intensity) = 0;
};