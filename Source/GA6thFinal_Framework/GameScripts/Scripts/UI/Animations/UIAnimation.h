#pragma once

class UIAnimation
{
public:
    using Callback = std::function<void(float alpha)>;

public:
    UIAnimation(const Callback& callback);

    void SetDuration(float duration);
    void SetLoop(bool isLoop);
    void SetElapsedTime(float elapsedTime);
    float GetElapsedTime() const { return _elapsedTime; }   
    void Reset();

    void Update(float deltaTime);

private:
    void UpdateAnimation() const;

private:
    float    _elapsedTime;
    float    _duration;
    bool     _isLoop;
    Callback _callback;
    bool     _isStop;
};