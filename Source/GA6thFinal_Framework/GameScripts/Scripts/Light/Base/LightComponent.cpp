#include "pchScripts.h"
#include "LightComponent.h"

LightComponent::LightComponent() 
    :
    Component(Component::TYPE::LIGHT),
    _light(std::make_unique<Light>()),
    Lighting(*_light)
{
    Lighting.SetActive(&EnableInHierarchy);
    UmGraphics.RegisterComponent("Game", _light.get());
    if constexpr (IS_EDITOR)
    {
        UmGraphics.RegisterComponent("Editor", _light.get());
    }
}

LightComponent::~LightComponent() 
{
    _light->SetDestroy();
    _light.reset();
}

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
    _gizmo.SetIconTexture(SceneGizmo::DefaultIcon::LIGHT);
}

void LightComponent::OnDrawDebug() 
{
    _gizmo.Draw();
}

