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

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(MainMenuNavigationBase)
    REFLECT_FIELDS_END(Credits)
};

