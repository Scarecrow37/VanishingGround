#pragma once

class GraphicsBase
{
    friend class RenderScene;
    friend class LightCore;
    friend class AnimationCore;

public:
    GraphicsBase() = default;
    virtual ~GraphicsBase();

public:
    bool IsActive() const { return _isActive ? *_isActive : false; }

public:
    void SetActive(const bool* isActive) { _isActive = isActive; }
    void SetDestroy();

private:
    std::vector<bool*> _isDestroyeds;
    const bool*        _isActive{nullptr};
};