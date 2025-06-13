#pragma once

class Light;
class LightCore
{
public:
    using LightComponent = std::pair<std::unique_ptr<bool>, std::shared_ptr<Light>>;

public:
    LightCore();
    ~LightCore();

public:
    const std::vector<LightComponent>& GetLights(std::string_view sceneName);

public:
    void RegisterLight(std::string_view sceneName, std::shared_ptr<Light> light);

public:
    void Update(const float deltaTime);

private:    
    std::unordered_map<std::string, std::vector<LightComponent>> _lights;
};