#pragma once
#include "UmFramework.h"
class RevelationSystem : public Component
{
    USING_PROPERTY(RevelationSystem)      
public:
    static RevelationSystem* GetInstance() { return static_instance; }

public:
    RevelationSystem();
    ~RevelationSystem() override;

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(RevelationSystem)

    void Reset() override;

private:
    inline static RevelationSystem* static_instance = nullptr;

};
