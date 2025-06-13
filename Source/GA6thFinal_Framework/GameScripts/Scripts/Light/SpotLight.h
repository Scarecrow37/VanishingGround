#pragma once
#include "PointLight.h"

class SpotLight : public PointLight
{
    USING_PROPERTY(SpotLight)
public:
    REFLECT_PROPERTY(
        ReflectFields->Inner, 
        ReflectFields->Outer
        )

public:
    SpotLight();
    virtual ~SpotLight();

protected:
    REFLECT_FIELDS_BEGIN(PointLight)
    float Inner = 0.f;
    float Outer = 100.f;
    REFLECT_FIELDS_END(SpotLight)

    /*
    역직렬화 이후 자동으로 호출되는 이벤트 함수 입니다.
    직접 override 해서 사용합니다.
    */
    virtual void DeserializedReflectEvent() override;

    virtual void Reset() override;
};
