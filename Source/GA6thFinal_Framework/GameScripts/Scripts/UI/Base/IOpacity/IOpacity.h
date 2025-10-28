#pragma once

class IOpacity
{
public:
    IOpacity()                           = default;
    IOpacity(const IOpacity&)            = default;
    IOpacity& operator=(const IOpacity&) = default;
    IOpacity(IOpacity&&)                 = default;
    IOpacity& operator=(IOpacity&&)      = default;
    virtual ~IOpacity()                  = default;

    virtual void SetOpacity(float opacity) = 0;
};