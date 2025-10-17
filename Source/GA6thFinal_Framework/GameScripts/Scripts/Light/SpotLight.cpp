#include "pchScripts.h"
#include "SpotLight.h"
#include "GraphicsEngine/Interface/ILight.h"

UMREAL_COMPONENT(SpotLight)

SpotLight::SpotLight() = default;

SpotLight::~SpotLight() = default;

void SpotLight::Reset() 
{
    Base::Reset();

    const Vector3& color = GetColor();
    const Vector3& position = transform->WorldPosition;
    const Vector3& direction = transform->Forward;
    const Vector3& attenuation = GetAttenuation();
    const float&   range       = GetRange();
    const float&   intensity   = GetIntensity();
    const float&   inner       = ReflectFields->Inner;
    const float&   outer      = ReflectFields->Outer;
    Lighting->SetSpotLight(color, position, direction, attenuation, range, inner, outer, intensity);
}

void SpotLight::OnDrawDebug() 
{
    Base::OnDrawDebug();
}

void SpotLight::OnDrawDebugSelected() 
{
    Vector3 position  = transform->WorldPosition;
    Vector3 direction = transform->Forward;
    UmGraphics.DebugDraw3D("Editor", position, direction, GetRange(), XMConvertToRadians(ReflectFields->Inner), XMConvertToRadians(ReflectFields->Outer), LightComponent::DEBUG_COLOR);
}
