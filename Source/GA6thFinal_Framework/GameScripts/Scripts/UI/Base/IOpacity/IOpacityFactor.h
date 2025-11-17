#pragma once

class IOpacityFactor
{
public:
    IOpacityFactor()                                 = default;
    IOpacityFactor(const IOpacityFactor&)            = default;
    IOpacityFactor& operator=(const IOpacityFactor&) = default;
    IOpacityFactor(IOpacityFactor&&)                 = default;
    IOpacityFactor& operator=(IOpacityFactor&&)      = default;
    virtual ~IOpacityFactor()                        = default;

    virtual void SetOpacityFactor(float factor) = 0;
};