#pragma once

class UIAnimation
{
public:
    using Callback = std::function<void(float alpha)>;

public:
    UIAnimation(const Callback& callback);

    void Reset(float duration, bool isLoop);

    void Update(float deltaTime);

private:
    float    _elapsedTime;
    float    _duration;
    bool     _isLoop;
    Callback _callback;
    bool     _isStop;
};