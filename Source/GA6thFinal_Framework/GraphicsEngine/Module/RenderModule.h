#pragma once

class RenderModule abstract
{
    friend class ModuleManager;

public:
    RenderModule() = default;
    virtual ~RenderModule() = default;

public:
    virtual void Initialize() = 0;
};