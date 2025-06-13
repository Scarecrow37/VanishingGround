#include "PointLight.h"
#include "Engine/GraphicsCore/Light.h"

PointLight::PointLight()
{
    Vector3 color = Vector3(ReflectFields->Basefields.get().Color.data());
    Vector3 attenuation = Vector3(ReflectFields->Constant, ReflectFields->Linear, ReflectFields->Quadratic);
    float range = ReflectFields->Range;
    float intensity = ReflectFields->Basefields.get().Intensity;

    _light->SetPointLight(
    color, 
    transform->Position, 
    attenuation, 
    range, 
    intensity
    );
}
PointLight::~PointLight() = default;

void PointLight::FixedUpdate() 
{
    _light->SetColor(Vector3(ReflectFields->Basefields.get().Color.data()));
    _light->SetPosition(transform->Position);
    _light->SetAttenuation(ReflectFields->Constant, ReflectFields->Linear, ReflectFields->Quadratic);
    _light->SetRange(ReflectFields->Range);
    _light->SetIntensity(ReflectFields->Basefields.get().Intensity);
}

