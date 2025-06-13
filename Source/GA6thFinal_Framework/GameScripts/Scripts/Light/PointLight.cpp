#include "PointLight.h"
#include "Engine/GraphicsCore/Light.h"

PointLight::PointLight()
{
    Vector3 color = Vector3(ReflectFields->Basefields.get().Color.data());
    Vector3 attenuation = Vector3(ReflectFields->Constant, ReflectFields->Linear, ReflectFields->Quadratic);
    float range = ReflectFields->Range;
    float intensity = ReflectFields->Basefields.get().Intensity;

    Lighting.SetPointLight(
    color, 
    transform->Position, 
    attenuation, 
    range, 
    intensity
    );
}
PointLight::~PointLight() = default;
