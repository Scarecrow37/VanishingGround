#include "pchScripts.h"
#include "SpotLight.h"
SpotLight::SpotLight() = default;
SpotLight::~SpotLight() = default;

void SpotLight::DeserializedReflectEvent() 
{

}

void SpotLight::Reset() 
{
    const Vector3& color = GetColor();
    const Vector3& position = transform->Position;
    const Vector3& direction = transform->Forward;
    const Vector3& attenuation = GetAttenuation();
    const float&   range       = GetRange();
    const float&   intensity   = GetIntensity();
    const float&   inner       = ReflectFields->Inner;
    const float&   outer      = ReflectFields->Outer;
    Lighting.SetSpotLight(color, position, direction, attenuation, range, inner, outer, intensity);
}
