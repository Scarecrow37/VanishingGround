#include "pchScripts.h"
#include "AutoRotateComponent.h"

UMREAL_COMPONENT(AutoRotateComponent)

AutoRotateComponent::AutoRotateComponent() = default;
AutoRotateComponent::~AutoRotateComponent() = default;

void AutoRotateComponent::Reset()
{
    Base::Reset();
}

void AutoRotateComponent::Update()
{
    float delta = UmTime.DeltaTime();
    
    if (ReflectFields->RotateSpeedX != 0.f)
    {
        transform->Rotate(Vector3(1, 0, 0), ReflectFields->RotateSpeedX * delta, Transform::Space::WORLD);
    }
    
    if (ReflectFields->RotateSpeedY != 0.f)
    {
        transform->Rotate(Vector3(0, 1, 0), ReflectFields->RotateSpeedY * delta, Transform::Space::WORLD);
    }
    
    if (ReflectFields->RotateSpeedZ != 0.f)
    {
        transform->Rotate(Vector3(0, 0, 1), ReflectFields->RotateSpeedZ * delta, Transform::Space::WORLD);
    }
}
