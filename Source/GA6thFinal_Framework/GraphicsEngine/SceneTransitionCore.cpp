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

void SceneTransitionCore::Fade(std::string_view sceneName, const float duration, const Vector4& start, const Vector4& end, const float maintain)
{
    std::string scenename(sceneName);
    _sceneTransitionProps[scenename]                  = new SceneTransitionProperty();
    _sceneTransitionProps[scenename]->_fadeDuration   = duration;
    _sceneTransitionProps[scenename]->_fadeStartColor = start;
    _sceneTransitionProps[scenename]->_fadeEndColor   = end;
    _sceneTransitionProps[scenename]->_fadeFlag       = true;
    _sceneTransitionProps[scenename]->_fadeMaintain       = maintain;
}
