#include "DirectionalLight.h"
#include "Engine/GraphicsCore/Light.h"

DirectionalLight::DirectionalLight()
{

}

DirectionalLight::~DirectionalLight() 
{

}

void DirectionalLight::DeserializedReflectEvent() 
{
    Ambient = Color(ReflectFields->Ambient[0], 
                     ReflectFields->Ambient[1], 
                     ReflectFields->Ambient[2],
                     ReflectFields->Ambient[3]);
}

void DirectionalLight::Reset() 
{
    const Vector3& color     = GetColor();
    const Vector3& ambient   = _ambientVector3;
    const Vector3& direction = transform->Forward;
    Lighting.SetDirectionalLight(color, ambient, direction, ReflectFields->Basefields.get().Intensity);
}


