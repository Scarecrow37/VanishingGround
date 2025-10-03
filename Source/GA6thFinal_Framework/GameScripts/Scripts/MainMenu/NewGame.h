#pragma once
#include "MainMenuNavigationBase.h"

class NewGame : public MainMenuNavigationBase
{
    USING_PROPERTY(NewGame)

public:
    NewGame();
    ~NewGame() override;

public:
    void Submit() override;
    void Update() override;

private:
    void TransitionToNextScene();

public:
    REFLECT_PROPERTY(NextScene)
    GETTER_ONLY(std::string, NextScene) { return File::Guid(ReflectFields->NextSceneGuid).ToPath().string(); }
    PROPERTY(NextScene)

protected:
    REFLECT_FIELDS_BEGIN(MainMenuNavigationBase)
    std::string NextSceneGuid;
    REFLECT_FIELDS_END(NewGame)
};