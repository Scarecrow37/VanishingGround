#pragma once

class UIGroup
{
public:
    virtual bool FindUI()                = 0;
    virtual bool IsValid() const         = 0;
    virtual void ActiveUI(bool active)   = 0;
    virtual void FadeIn(float duration)  = 0;
    virtual void FadeOut(float duration) = 0;
};
