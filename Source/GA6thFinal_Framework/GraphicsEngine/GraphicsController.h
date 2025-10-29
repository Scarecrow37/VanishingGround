#pragma once

class GraphicsController
{
public:
    GraphicsController() = default;
    ~GraphicsController();

public:
    void SetSSR(std::string_view sceneName, bool enable);
    void SetSSAO(std::string_view sceneName, bool enable);
    void SetVolumetricFog(std::string_view sceneName, bool enable);
    void SetBloom(std::string_view sceneName, bool eanble);
    void SetTextureQuality(std::string_view sceneName, float quality);
};