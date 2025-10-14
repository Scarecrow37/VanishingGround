#include "pchScripts.h"
#include "PointLight.h"
#include "GraphicsEngine/Interface/ILight.h"

UMREAL_COMPONENT(PointLight)

PointLight::PointLight()  = default;

PointLight::~PointLight() = default;

void PointLight::DeserializedReflectEvent()
{
    _attenuation = Vector3(ReflectFields->Attenuation.data());
}

void PointLight::Reset() 
{
    Base::Reset();

    const Vector3& color       = GetColor();
    const Vector3& attenuation = _attenuation;
    float&         range       = ReflectFields->Range;
    float&         intensity   = ReflectFields->Basefields.get().Intensity;

    Lighting->SetPointLight(color, transform->WorldPosition, attenuation, range, intensity);
}

void PointLight::OnDrawDebug() 
{
    Base::OnDrawDebug();
}

void PointLight::OnDrawDebugSelected() 
{
    BoundingSphere sphere;
    sphere.Center = transform->WorldPosition;
    sphere.Radius = ReflectFields->Range;
    UmGraphics.DebugDraw3D("Editor", sphere, LightComponent::DEBUG_COLOR);
}
