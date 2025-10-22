#include "pchScripts.h"
#include "GraphicsEngine/Interface/ILight.h"
#include "ShadowPointLight.h"

UMREAL_COMPONENT(ShadowPointLight)

ShadowPointLight::ShadowPointLight() = default;

ShadowPointLight::~ShadowPointLight() = default;

void ShadowPointLight::DeserializedReflectEvent()
{
    _attenuation = Vector3(ReflectFields->Attenuation.data());
}

void ShadowPointLight::Reset()
{
    Base::Reset();

    const Vector3& color       = GetColor();
    const Vector3& attenuation = _attenuation;
    float&         range       = ReflectFields->Range;
    float&         intensity   = ReflectFields->Basefields.get().Intensity;

    Lighting->SetShadowPointLight(color, transform->WorldPosition, attenuation, range, intensity);
}

void ShadowPointLight::OnDrawDebug()
{
    Base::OnDrawDebug();
}

void ShadowPointLight::OnDrawDebugSelected()
{
    BoundingSphere sphere;
    sphere.Center = transform->WorldPosition;
    sphere.Radius = ReflectFields->Range;
    UmGraphics.DebugDraw3D("Editor", sphere, LightComponent::DEBUG_COLOR);
}
