#include "pch.h"
#include "LightCore.h"

LightCore::LightCore() {}

LightCore::~LightCore() {}

std::vector<std::shared_ptr<Light>>& LightCore::GetLights(std::string_view sceneName)
{
    return _lights[sceneName.data()];
}

void LightCore::RegisterLight(std::string_view sceneName, std::shared_ptr<Light> light)
{
    _lights[sceneName.data()].push_back(light);
}

void LightCore::UnRegisterLight(std::string_view sceneName, std::shared_ptr<Light> light)
{
}