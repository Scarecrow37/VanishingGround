#include "pch.h"
#include "LightCore.h"
#include "Light.h"

LightCore::LightCore() = default;

LightCore::~LightCore() = default;

const std::vector<Light*>& LightCore::GetLights(std::string_view sceneName)
{
    return _lights[sceneName.data()];
}

void LightCore::RegisterLight(std::string_view sceneName, Light* light)
{
    auto& lights = _lights[sceneName.data()];

    auto iter = std::find_if(lights.begin(), lights.end(), [light](const auto& component) { return component->GetID() == light->GetID(); });

    if (iter != lights.end())
    {
        //GRAPHICS_ASSERT(false, L"LightCore::RegisterLight : Already registered light.");
        return;
    }

    lights.push_back(light);
}

void LightCore::ClearLightQueue()
{
    _lights.clear();
}

void LightCore::UpdateLightQueue()
{
    for (auto& [sceneName, lights] : _lights)
    {
        auto first = std::remove_if(lights.begin(), lights.end(), [](const auto& light) { return !light->IsAlive(); });
        lights.erase(first, lights.end());
    }
}

void LightCore::Update(const float deltaTime)
{
    for (auto& [sceneName, lights] : _lights)
    {
        for (auto& light : lights)
        {
            if (!light->IsActive() || !light->IsAlive())
                continue;

            light->Update(deltaTime);
        }
    }    
}