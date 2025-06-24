#pragma once

class GraphicsBase
{
    friend class RenderScene;
    friend class LightCore;

public:
    GraphicsBase()          = default;
    virtual ~GraphicsBase() = default;

public:
    bool IsActive() const { return _isActive; }

public:
    void SetActive(bool isActive) { _isActive = isActive; }
    void SetDestroy();

private:
    bool  _isActive  = false;
    std::vector<bool*> _isDestroyeds;
};