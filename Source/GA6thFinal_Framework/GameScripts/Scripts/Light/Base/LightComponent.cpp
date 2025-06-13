#include "LightComponent.h"

LightComponent::LightComponent() 
    :
    Component(Component::Type::Light),
    _light(std::make_unique<Light>()),
    Lighting(*_light)
{
    LightCore& lightCore = UmLightCore;
    lightCore.RegisterLight("Editor", _light.get());
}

LightComponent::~LightComponent() 
{
    LightCore& lightCore = UmLightCore;
    _light->SetDestroy();
    _light.reset();
}

void LightComponent::DeserializedReflectEvent() 
{
    LightColor = Color(ReflectFields->Color[0], 
                        ReflectFields->Color[1], 
                        ReflectFields->Color[2], 
                        ReflectFields->Color[3]);
}

