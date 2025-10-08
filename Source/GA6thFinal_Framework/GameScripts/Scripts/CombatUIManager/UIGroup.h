#pragma once

class UIGroup
{
    virtual bool FindUI() = 0;
    virtual bool IsValid() = 0;
    virtual void ActiveUI(bool active) = 0;
};
