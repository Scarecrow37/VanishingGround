#pragma once
struct SceneTransitionProperty
{
    Color _fadeColor = {0, 0, 0, 0};
    bool  _fadeFlag       = false;
};

class SceneTransitionCore
{
public:
    SceneTransitionCore();
    virtual ~SceneTransitionCore();

    void Fade(const char* sceneName, const Color& color, bool flag);

public:
    std::unordered_map<std::string, SceneTransitionProperty> _sceneTransitionProps;
};