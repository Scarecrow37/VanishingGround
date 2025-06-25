#include "pchScripts.h"
#include "LightComponent.h"

LightComponent::LightComponent() 
    :
    Component(Component::TYPE::LIGHT),
    _light(std::make_unique<Light>()),
    Lighting(*_light)
{
    LightCore& lightCore = UmLightCore;
    lightCore.RegisterLight("Editor", _light.get());
    lightCore.RegisterLight("Game", _light.get());
    Lighting.SetActive(true);
}

LightComponent::~LightComponent() 
{
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

void LightComponent::Reset() 
{

}

