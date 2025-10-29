#include "pchScripts.h"
#include "UIAnimation.h"


UIAnimation::UIAnimation(const Callback& callback)
    : _elapsedTime(0.0f), _duration(0.1f), _isLoop(false), _callback(callback), _isStop(false)
{
}

void UIAnimation::SetDuration(const float duration)
{
    _duration = duration;
}

void UIAnimation::SetLoop(const bool isLoop)
{
    _isLoop = isLoop;
}

void UIAnimation::SetElapsedTime(const float elapsedTime)
{
    _elapsedTime = elapsedTime;
    UpdateAnimation();
}

void UIAnimation::Reset()
{
    _isStop = false;
}

void UIAnimation::Update(const float deltaTime)
{
    if (_isStop)
        return;

    _elapsedTime += deltaTime;

    UpdateAnimation();

    const bool isOverDuration = _elapsedTime >= _duration;
    const bool isUnderZero    = _elapsedTime <= 0.0f;

    if (const bool isOutOfRange = isOverDuration || isUnderZero; isOutOfRange)
    {
        if (_isLoop)
        {
            _elapsedTime = isOverDuration ? _elapsedTime - _duration : _elapsedTime + _duration;
        }
        else
        {
            _elapsedTime = std::clamp(_elapsedTime, 0.0f, _duration);
            _isStop = true;
        } 
    }
}

void UIAnimation::UpdateAnimation() const
{
    const float alpha = std::clamp(_elapsedTime / _duration, 0.0f, 1.0f);

    if (_callback)
        _callback(alpha);
}
