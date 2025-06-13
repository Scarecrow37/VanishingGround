#pragma once
#include "Base/LightComponent.h"
class DirectionalLight : public LightComponent
{
    USING_PROPERTY(DirectionalLight)
public:
    REFLECT_PROPERTY(
        ReflectFields->Ambient,
        ReflectFields->Direction)

public:
    DirectionalLight();
    virtual ~DirectionalLight();

protected:
    REFLECT_FIELDS_BEGIN(LightComponent)
    std::array<float, 3> Ambient{1.f, 1.f, 1.f};
    std::array<float, 3> Direction{0.f, -1.f, 1.f};
    REFLECT_FIELDS_END(DirectionalLight)

    virtual void FixedUpdate() override;
};
