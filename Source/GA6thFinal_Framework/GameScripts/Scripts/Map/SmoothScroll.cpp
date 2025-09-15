#include "pchScripts.h"
#include "SmoothScroll.h"

#include "UI/Wrappers/Scrolling/ScrollingWrapper.h"

SmoothScroll::SmoothScroll() = default;
SmoothScroll::~SmoothScroll() = default;

void SmoothScroll::SetScrollTarget(float target)
{
    _target  = std::clamp(target, 0.f, 1.f);
    _elapsed = 0.f;
}

void SmoothScroll::Awake()
{
    _scroll = GetComponent<ScrollingWrapper>();
}

void SmoothScroll::Update()
{
    if (_scroll == nullptr)
        return;

    if (1.f <= _elapsed)
        return;

    _elapsed += UmTime.DeltaTime() * _speed;
    _scroll->Scroll = std::lerp(_scroll->Scroll, _target, _elapsed);
}