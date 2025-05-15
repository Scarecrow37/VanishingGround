#pragma once

class RendererBase
{
public:
    RendererBase() = default;
    virtual ~RendererBase() = default;

public:
    bool IsActive() const { return _isActive; }
    bool IsDestroy() const { return _isDestroy; }

public:
    void SetActive(bool isActive) { _isActive = isActive; }
    void SetDestroy() { _isDestroy = true; }

public:
    virtual void RegisterRenderQueue(std::string_view sceneName) = 0;

private:
    bool _isActive = false;
    bool _isDestroy = false;
};