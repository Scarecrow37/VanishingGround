#include "pchScripts.h"
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

void DirectionalLight::OnDrawDebug() 
{

}

void DirectionalLight::OnDrawDebugSelected() 
{
    constexpr float S = 0.70710678f;
    constexpr Vector3 DirectionRays8[8] = 
    {
        {1.0f, 0.0f, 0.0f},  // 0도   (오른쪽)
        {S, 0.0f, S},        // 45도  (오른쪽 위)
        {0.0f, 0.0f, 1.0f},  // 90도  (앞)
        {-S, 0.0f, S},       // 135도 (왼쪽 위)
        {-1.0f, 0.0f, 0.0f}, // 180도 (왼쪽)
        {-S, 0.0f, -S},      // 225도 (왼쪽 아래)
        {0.0f, 0.0f, -1.0f}, // 270도 (뒤)
        {S, 0.0f, -S},       // 315도 (오른쪽 아래)
    };
    const Vector3& position = transform->Position;
    const Vector3& dir      = transform->Forward;
    Vector3 rayPositions[8];
    for (int i = 0; i < 8; ++i)
    {
        rayPositions[i] = position + DirectionRays8[i] * DEBUG_LINE_RADIUS;
    }
    for (int i = 0; i < 8; ++i)
    {
        UmDebugDrawCore.DrawRay("Editor", rayPositions[i], dir * DEBUG_LINE_LENGTH, false, LightComponent::DEBUG_COLOR);
    }
}


