#include "pch.h"
#include "SceneTransitionCore.h"

SceneTransitionCore::SceneTransitionCore() = default;

SceneTransitionCore::~SceneTransitionCore() = default;

void SceneTransitionCore::Fade(const char* sceneName, const Color& color, bool flag)
{
    auto [it, inserted]  = _sceneTransitionProps.try_emplace(sceneName);
    auto& prop          = it->second;
    prop._fadeColor = color;
    prop._fadeFlag      = flag;
}
