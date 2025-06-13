#include "DirectionalLight.h"
#include "Engine/GraphicsCore/Light.h"

DirectionalLight::DirectionalLight()
{
    Vector3 color(ReflectFields->Basefields.get().Color.data());
    Vector3 ambient(ReflectFields->Ambient.data());
    Vector3 direction(ReflectFields->Direction.data());
    _light->SetDirectionalLight(color, ambient, direction, ReflectFields->Basefields.get().Intensity);
}

DirectionalLight::~DirectionalLight() 
{

}
void DirectionalLight::FixedUpdate()
{
    _light->SetDirection(Vector3(ReflectFields->Direction.data()));
    _light->SetColor(Vector3(ReflectFields->Basefields.get().Color.data()));
    _light->SetIntensity(ReflectFields->Basefields.get().Intensity);
}
