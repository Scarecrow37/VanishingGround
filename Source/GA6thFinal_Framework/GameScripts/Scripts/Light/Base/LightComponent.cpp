#include "pchScripts.h"
#include "LightComponent.h"

LightComponent::LightComponent() 
    :
    Component(Component::TYPE::LIGHT),
    _light(std::make_unique<Light>()),
    Lighting(*_light)
{
    Lighting.RegisterComponent();
    Lighting.SetActive(&EnableInHierarchy);
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

