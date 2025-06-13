#pragma once

class Light;
class LightCore
{
public:
    LightCore();
    ~LightCore();

public:
    std::vector<std::shared_ptr<Light>>& GetLights(std::string_view sceneName);

public:
    void RegisterLight(std::string_view sceneName, std::shared_ptr<Light> light);
    void UnRegisterLight(std::string_view sceneName, std::shared_ptr<Light> light);

private:
    std::unordered_map<std::string, std::vector<std::shared_ptr<Light>>> _lights;
};