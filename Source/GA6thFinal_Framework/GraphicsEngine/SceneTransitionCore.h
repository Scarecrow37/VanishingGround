#pragma once

struct SceneTransitionProperty
{
    float   _fadeDuration   = -1;
    Vector4 _fadeStartColor = {0, 0, 0, 0};
    Vector4 _fadeEndColor   = {0, 0, 0, 0};
    bool    _fadeFlag       = false;
    float   _fadeMaintain   = 0;
};

class SceneTransitionCore
{
public:
    SceneTransitionCore();
    virtual ~SceneTransitionCore();

    void Fade(std::string_view sceneName, const float duration, const Vector4& start, const Vector4& end,
              const float maintain);

public:
    std::unordered_map<std::string, SceneTransitionProperty*> _sceneTransitionProps;
};