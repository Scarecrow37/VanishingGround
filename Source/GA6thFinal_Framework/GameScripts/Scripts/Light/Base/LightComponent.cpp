#include "LightComponent.h"

LightComponent::LightComponent() 
    :
    Component(Component::Type::Light),
    _light(std::make_shared<Light>()),
    Lighting(*_light)
{
    LightCore& lightCore = UmLightCore;
    lightCore.RegisterLight("Editor", _light);
}

LightComponent::~LightComponent() 
{
    LightCore& lightCore = UmLightCore;
    lightCore.UnRegisterLight("Editor", _light);
    _light.reset();
}

void LightComponent::DeserializedReflectEvent() 
{
    _lightColor = Color(ReflectFields->Color[0], 
                        ReflectFields->Color[1], 
                        ReflectFields->Color[2], 
                        ReflectFields->Color[3]);
}

