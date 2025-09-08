#include "pch.h"
#include "SceneTransitionCore.h"

SceneTransitionCore::SceneTransitionCore() {}

SceneTransitionCore::~SceneTransitionCore() 
{
    for (auto prop : _sceneTransitionProps)
    {
        delete prop.second;
    }
}

void SceneTransitionCore::Fade(std::string_view sceneName, float duration, const Vector4& start, const Vector4& end)
{
    std::string scenename(sceneName);
    _sceneTransitionProps[scenename]                  = new SceneTransitionProperty();
    _sceneTransitionProps[scenename]->_fadeDuration   = duration;
    _sceneTransitionProps[scenename]->_fadeStartColor = start;
    _sceneTransitionProps[scenename]->_fadeEndColor   = end;
    _sceneTransitionProps[scenename]->_fadeFlag       = true;
}
