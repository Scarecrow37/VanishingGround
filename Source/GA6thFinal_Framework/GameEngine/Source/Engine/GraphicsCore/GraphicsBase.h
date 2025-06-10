#pragma once

class GraphicsBase
{
    friend class RenderScene;

public:
    GraphicsBase()          = default;
    virtual ~GraphicsBase() = default;

public:
    bool IsActive() const { return _isActive; }
    bool IsDestroy() const { return *_isDestroy; }

public:
    void SetActive(bool isActive) { _isActive = isActive; }
    void SetDestroy() { *_isDestroy = true; }

private:
    bool  _isActive  = false;
    bool* _isDestroy = nullptr;
};