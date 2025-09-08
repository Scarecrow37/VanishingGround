#include "pch.h"
#include "PreferencesSystem.h"
#include "../GraphicsEngine/GraphicsConstroller.h"

PreferencesSystem::PreferencesSystem() {}

PreferencesSystem::~PreferencesSystem() {}

void PreferencesSystem::Initialize()
{
    _graphicsConstroller = std::make_unique<class GraphicsConstroller>();
}

void PreferencesSystem::SetSSR(bool enable) 
{
    _graphicsConstroller->SetSSR("Game", enable);
}
void PreferencesSystem::SetSSR(std::string_view sceneName, bool enable)
{
    _graphicsConstroller->SetSSR(sceneName, enable);
}

void PreferencesSystem::SetSSAO(bool enable) {}

void PreferencesSystem::SetSSAO(std::string_view sceneName, bool enable) {}

void PreferencesSystem::SetVolumetricFog(bool enable) {}

void PreferencesSystem::SetVolumetricFog(std::string_view sceneName, bool enable) {}

void PreferencesSystem::SetBloom(bool enable) {}

void PreferencesSystem::SetBloom(std::string_view sceneName, bool enable) {}

void PreferencesSystem::SetRayTracing(bool enable) {}

void PreferencesSystem::SetRayTracing(std::string_view sceneName, bool enable) {}

void PreferencesSystem::SetTextureQuality(TextureQuality quality) {}

void PreferencesSystem::SetTextureQuality(std::string_view sceneName, TextureQuality quality) {}

void PreferencesSystem::SetShadowQuality(int quality) {}

void PreferencesSystem::SetShadowQuality(std::string_view sceneName, int quality) {}
