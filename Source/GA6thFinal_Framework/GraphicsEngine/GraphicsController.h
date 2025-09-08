#pragma once
class GraphicsController
{
public:
    GraphicsController() = default;
    ~GraphicsController();

public:
    void SetSSR(std::string_view sceneName, bool enable);
    void SetSSAO(std::string_view sceneName, bool enable);
};
