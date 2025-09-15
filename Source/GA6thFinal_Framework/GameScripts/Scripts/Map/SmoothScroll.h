#pragma once
#include "UmFramework.h"

class ScrollingWrapper;
class SmoothScroll : public Component
{
    USING_PROPERTY(SmoothScroll)

public:
    SmoothScroll();
    ~SmoothScroll() override;

public:
    void SetScrollTarget(float target);

public:
    void Awake() override;
    void Update() override;

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(SmoothScroll)

private:
    ScrollingWrapper* _scroll = nullptr;
    float             _speed  = 10.f;
    float             _target = 0.f;
    float             _elapsed = 0.f;
};