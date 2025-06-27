#include "pchScripts.h"
#include "PointLight.h"
#include "Engine/GraphicsCore/Light.h"

PointLight::PointLight()
{
   
}
PointLight::~PointLight() = default;

void PointLight::Update()
{
    _boundingSphere.Center = transform->Position;
    _boundingSphere.Radius = ReflectFields->Range;

    UmDebugDrawCore.Draw("Editor", _boundingSphere, DirectX::Colors::GreenYellow);
}

void PointLight::DeserializedReflectEvent()
{
    _attenuation = Vector3(ReflectFields->Attenuation.data());
}

void PointLight::Reset() 
{
    const Vector3& color       = GetColor();
    const Vector3& attenuation = _attenuation;
    float&         range       = ReflectFields->Range;
    float&         intensity   = ReflectFields->Basefields.get().Intensity;

    Lighting.SetPointLight(color, transform->Position, attenuation, range, intensity);
}
