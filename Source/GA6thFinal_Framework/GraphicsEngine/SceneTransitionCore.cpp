#include "pch.h"
#include "SceneTransitionCore.h"

SceneTransitionCore::SceneTransitionCore() = default;

SceneTransitionCore::~SceneTransitionCore() = default;

void SceneTransitionCore::Fade(const char* sceneName, const float duration, const Vector4& start, const Vector4& end,
                               const float maintain)
{
    auto [it, inserted]  = _sceneTransitionProps.try_emplace(sceneName);
    auto& prop           = it->second;
    prop._fadeDuration   = duration;
    prop._fadeStartColor = start;
    prop._fadeEndColor   = end;
    prop._fadeFlag       = true;
    prop._fadeMaintain   = maintain;
}
