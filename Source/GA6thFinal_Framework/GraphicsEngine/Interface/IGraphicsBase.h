#pragma once

class IGraphicsBase
{    
public:
    virtual bool IsActive() const = 0;

public:
    virtual void SetActive(const bool* isActive) = 0;

public:
    virtual void AddReference() = 0;
    virtual void Release() = 0;
};