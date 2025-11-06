#pragma once
#include "MainMenuNavigationBase.h"

class Credits : public MainMenuNavigationBase
{
    USING_PROPERTY(Credits)

public:
    Credits();
    ~Credits() override;

public:
    void Submit() override;

private:
    void TransitionToCreditsScene();

public:
    REFLECT_PROPERTY(NextScene)
    GETTER_ONLY(std::string, NextScene) { return File::Guid(ReflectFields->NextSceneGuid).ToPath().string(); }
    PROPERTY(NextScene)
protected:
    REFLECT_FIELDS_BEGIN(MainMenuNavigationBase)
    std::string NextSceneGuid;
    REFLECT_FIELDS_END(Credits)
};

