#include "pchScripts.h"
#include "UIAnimation.h"

void UIAnimation::Reset(const float duration, const bool isLoop)
{
    _elapsedTime = 0.0f;
    _isLoop      = isLoop;

    if (duration <= 0.0f)
    {
        _duration = 0.1f;
        _isStop   = true;
    }
    else
    {
        _duration = duration;
        _isStop   = false;
    }
}

UIAnimation::UIAnimation(const Callback& callback)
    : _elapsedTime(0.0f), _duration(0.1f), _isLoop(false), _callback(callback), _isStop(false)
{
}

void UIAnimation::Update(const float deltaTime)
{
    if (_isStop)
        return;

    _elapsedTime += deltaTime;

    const float alpha = std::clamp(_elapsedTime / _duration, 0.0f, 1.0f);

    if (_callback)
        _callback(alpha);

    if (_elapsedTime >= _duration)
    {
        if (_isLoop)
            _elapsedTime -= _duration;
        else
            _isStop = true;
    }
}