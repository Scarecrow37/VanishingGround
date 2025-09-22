#pragma once
#include "MainMenuNavigationBase.h"

class Quit : public MainMenuNavigationBase
{
    USING_PROPERTY(Quit)

public:
    Quit();
    ~Quit() override;

public:
    void Submit() override;

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(MainMenuNavigationBase)
    REFLECT_FIELDS_END(Quit)
};

