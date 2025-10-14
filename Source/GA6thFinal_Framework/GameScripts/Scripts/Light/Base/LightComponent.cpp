#include "pchScripts.h"
#include "LightComponent.h"
#include "GraphicsEngine/Interface/ILight.h"

LightComponent::LightComponent() 
    : Component(Component::TYPE::LIGHT)
    , _light(nullptr)
    , Lighting(_light)
{    
}

LightComponent::~LightComponent() = default;

void LightComponent::DeserializedReflectEvent() 
{
    LightColor = Color(ReflectFields->Color[0], 
                       ReflectFields->Color[1], 
                       ReflectFields->Color[2], 
                       ReflectFields->Color[3]);
}

void LightComponent::Reset() 
{
    Base::Reset();
    UmGraphics.CreateLight(&_light);
    Lighting->SetActive(&EnableInHierarchy);

    UmGraphics.RegisterComponent("Game", _light.Get());
    if constexpr (IS_EDITOR)
    {
        UmGraphics.RegisterComponent("Editor", _light.Get());
    }

#ifdef _UMEDITOR
    _gizmo.SetIconTexture(SceneGizmo::DefaultIcon::LIGHT);
#endif
}

void LightComponent::OnDrawDebug() 
{
#ifdef _UMEDITOR
    _gizmo.DrawIcon();
#endif
}

