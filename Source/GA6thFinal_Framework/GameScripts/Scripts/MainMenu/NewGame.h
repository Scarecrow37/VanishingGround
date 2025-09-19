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

public:
    REFLECT_PROPERTY(NextScene)
    GETTER_ONLY(std::string, NextScene) { return ReflectFields->NextScene; }
    PROPERTY(NextScene)

protected:
    REFLECT_FIELDS_BEGIN(MainMenuNavigationBase)
    std::string NextScene;
    REFLECT_FIELDS_END(NewGame)
};