#pragma once

class Light;
class LightCore
{
public:
    LightCore();
    ~LightCore();

public:
    const std::vector<Light*>& GetLights(std::string_view sceneName);

public:
    void RegisterLight(std::string_view sceneName, Light* light);
    void ClearLightQueue();

public:
    void Update(const float deltaTime);

private:    
    std::unordered_map<std::string, std::vector<Light*>> _lights;
};