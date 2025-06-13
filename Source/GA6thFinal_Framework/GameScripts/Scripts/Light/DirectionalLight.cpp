#include "DirectionalLight.h"
#include "Engine/GraphicsCore/Light.h"

DirectionalLight::DirectionalLight()
{
    Vector3 color(ReflectFields->Basefields.get().Color.data());
    Vector3 ambient(ReflectFields->Ambient.data());
    Vector3 direction(ReflectFields->Direction.data());
    Lighting.SetDirectionalLight(color, ambient, direction, ReflectFields->Basefields.get().Intensity);
}

DirectionalLight::~DirectionalLight() 
{

}

void DirectionalLight::DeserializedReflectEvent() 
{
    _ambient = Color(ReflectFields->Ambient[0], 
                     ReflectFields->Ambient[1], 
                     ReflectFields->Ambient[2],
                     ReflectFields->Ambient[3]);
}


