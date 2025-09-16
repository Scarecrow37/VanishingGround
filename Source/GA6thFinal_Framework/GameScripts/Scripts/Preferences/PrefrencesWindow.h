#pragma once
#include "UmFramework.h"
class PrefrencesWindow : public Component
{
    USING_PROPERTY(PrefrencesWindow)

public:
    PrefrencesWindow();
    ~PrefrencesWindow() override;

public:
    
public:
    REFLECT_PROPERTY()
        
protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(PrefrencesWindow)
private:
    MVVM::Model<bool> _preferencesOn = false;
};
