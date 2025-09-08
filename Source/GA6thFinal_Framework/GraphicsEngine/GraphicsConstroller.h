#pragma once
class GraphicsConstroller
{
public:
    GraphicsConstroller() = default;
    ~GraphicsConstroller();

public:
    void SetSSR(std::string_view sceneName, bool enable);
    void SetSSAO(std::string_view sceneName, bool enable);
};
