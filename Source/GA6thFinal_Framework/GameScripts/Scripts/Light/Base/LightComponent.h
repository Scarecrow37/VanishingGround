#pragma once
#include "UmFramework.h"
#include "Engine/GraphicsCore/Light.h"

class LightComponent : public Component
{
    USING_PROPERTY(LightComponent)
public:
    REFLECT_PROPERTY(
        LightColor,
        ReflectFields->Intensity)

    GETTER(const Color&, LightColor) 
    { 
        return _lightColor;
    }
    SETTER(const Color&, LightColor) 
    {
        _lightColor = value;
        std::memcpy(&_lightColor.x, &ReflectFields->Color[0], sizeof(ReflectFields->Color));
        std::memcpy(&_lightColor.x, &_lightColorVector3.x, sizeof(_lightColorVector3));
    }
    PROPERTY(LightColor)

    const Vector3& GetColor()
    {
        return _lightColorVector3;
    }

    GETTER(float, Intensity)
    {
        return ReflectFields->Intensity;
    }
    SETTER(float, Intensity) 
    { 
        ReflectFields->Intensity = value;
    }

private:
    std::unique_ptr<Light> _light;

public:
    LightComponent();
    virtual ~LightComponent() override;
    Light& Lighting;

protected:
    REFLECT_FIELDS_BEGIN(Component)
    float Intensity = 1.f;
    std::array<float, 4> Color{1.f, 1.f, 1.f, 1.f};    
    REFLECT_FIELDS_END(LightComponent)

    /*
    역직렬화 이후 자동으로 호출되는 이벤트 함수 입니다.
    직접 override 해서 사용합니다.
    */
    virtual void DeserializedReflectEvent() override;

private:
    Color _lightColor{1.f, 1.f, 1.f, 1.f};
    Vector3 _lightColorVector3{1.f, 1.f, 1.f};
};