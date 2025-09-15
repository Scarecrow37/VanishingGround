#include "pch.h"
#include "PreferencesSystem.h"
#include "../GraphicsEngine/GraphicsController.h"

PreferencesSystem::PreferencesSystem() {}

PreferencesSystem::~PreferencesSystem() {}

void PreferencesSystem::Initialize()
{
    _graphicsController = std::make_unique<class GraphicsController>();
}

void PreferencesSystem::SetSSR(bool enable) 
{
    _graphicsController->SetSSR("Game", enable);
}
void PreferencesSystem::SetSSR(std::string_view sceneName, bool enable)
{
    _graphicsController->SetSSR(sceneName, enable);
}

void PreferencesSystem::SetSSAO(bool enable)
{
    _graphicsController->SetSSAO("Game", enable);
}

void PreferencesSystem::SetSSAO(std::string_view sceneName, bool enable) 
{
    _graphicsController->SetSSAO(sceneName, enable);
}

void PreferencesSystem::SetVolumetricFog(bool enable) 
{
    _graphicsController->SetVolumetricFog("Game", enable);
}

void PreferencesSystem::SetVolumetricFog(std::string_view sceneName, bool enable) 
{
    _graphicsController->SetVolumetricFog(sceneName, enable);
}

void PreferencesSystem::SetBloom(bool enable) 
{
    _graphicsController->SetBloom("Game", enable);
}

void PreferencesSystem::SetBloom(std::string_view sceneName, bool enable) 
{
    _graphicsController->SetBloom(sceneName, enable);
}

void PreferencesSystem::SetRayTracing(bool enable) {}

void PreferencesSystem::SetRayTracing(std::string_view sceneName, bool enable) {}

void PreferencesSystem::SetTextureQuality(TextureQuality quality) 
{
    float bias = 0.f;
    switch (quality)
    {
    case PreferencesSystem::TextureQuality::LOW:
        bias = 3.f;  
        break;
    case PreferencesSystem::TextureQuality::MEDIUM:
        bias = 1.5f;
        break;
    case PreferencesSystem::TextureQuality::HIGH:
        bias = 0.f;
        break;
    default:
        break;
    }
    _graphicsController->SetTextureQuality("Game",bias);
}

void PreferencesSystem::SetTextureQuality(std::string_view sceneName, TextureQuality quality) 
{
    float bias = 0.f;
    switch (quality)
    {
    case PreferencesSystem::TextureQuality::LOW:
        bias = 3.f;
        break;
    case PreferencesSystem::TextureQuality::MEDIUM:
        bias = 1.5f;
        break;
    case PreferencesSystem::TextureQuality::HIGH:
        bias = 0.f;
        break;
    default:
        break;
    }
    _graphicsController->SetTextureQuality(sceneName, bias);
}

void PreferencesSystem::SetShadowQuality(int quality) {}

void PreferencesSystem::SetShadowQuality(std::string_view sceneName, int quality) {}
