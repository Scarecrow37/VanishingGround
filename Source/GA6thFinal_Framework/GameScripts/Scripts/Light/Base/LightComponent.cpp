#include "LightComponent.h"
#include "Engine/GraphicsCore/Light.h"

LightComponent::LightComponent() 
    :
    Component(Component::Type::Light) 
{
    LightCore& lightCore = UmLightCore;
    _light = std::make_shared<Light>();
    lightCore.RegisterLight("Editor", _light);
}

LightComponent::~LightComponent() 
{
    LightCore& lightCore = UmLightCore;
    lightCore.UnRegisterLight("Editor", _light);
    _light.reset();
}

