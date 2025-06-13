#include "pch.h"
#include "LightCore.h"
#include "Light.h"

LightCore::LightCore() {}

LightCore::~LightCore() {}

const std::vector<LightCore::LightComponent>& LightCore::GetLights(std::string_view sceneName)
{
    return _lights[sceneName.data()];
}

void LightCore::RegisterLight(std::string_view sceneName, std::shared_ptr<Light> light)
{
    auto& lights = _lights[sceneName.data()];

    auto iter = std::find_if(lights.begin(), lights.end(), [](const auto& pair) { return !pair.first.get(); });

    if (iter != lights.end())
    {
        GRAPHICS_ASSERT(false, L"LightCore::RegisterLight : Already registered light.");
        return;
    }

    lights.emplace_back(std::make_unique<bool>(false), light);
    light->_isDestroy = lights.back().first.get();
}

void LightCore::Update(const float deltaTime)
{
    for (auto& [sceneName, lights] : _lights)
    {
        auto first = std::remove_if(lights.begin(), lights.end(), [](const auto& pair) { return *pair.first; });
        lights.erase(first, lights.end());

        for (auto& [isDestroy, light] : lights)
        {
            light->Update(deltaTime);
        }
    }
}