#pragma once
#include "MainMenuNavigationBase.h"

class Option : public MainMenuNavigationBase
{
    USING_PROPERTY(Option)

public:
    Option();
    ~Option() override;

public:
    void Submit() override;

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(MainMenuNavigationBase)
    REFLECT_FIELDS_END(Option)
};

