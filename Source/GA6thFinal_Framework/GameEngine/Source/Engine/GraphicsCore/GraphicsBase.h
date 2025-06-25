#pragma once

class GraphicsBase
{
    friend class RenderScene;
    friend class LightCore;

public:
    GraphicsBase()          = default;
    virtual ~GraphicsBase() = default;

public:
    bool IsActive() const { return _isActive ? *_isActive : false; }

public:
    void SetActive(bool* isActive) { _isActive = isActive; }
    void SetDestroy();

private:
    std::vector<bool*> _isDestroyeds;
    bool*              _isActive{nullptr};
};