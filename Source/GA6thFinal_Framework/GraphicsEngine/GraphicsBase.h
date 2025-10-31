#pragma once

class GraphicsBase
{    
protected:
    GraphicsBase();
    virtual ~GraphicsBase() = default;

public:
    unsigned long long GetID() const { return _ID; }
    bool               IsAlive() const { return _referenceCount > 0; }
    bool               IsActive() const { return IsAlive() && (_isActive ? *_isActive : false); }

public:
    void SetActive(const bool* isActive) { _isActive = isActive; }

public:
    void AddReference();
    void Release();
    void Delete() { delete this; }

private:
    static unsigned long long _globalID;
    unsigned long long        _ID;
    unsigned long long        _referenceCount;
    const bool*               _isActive;
};