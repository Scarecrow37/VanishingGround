#pragma once
#include "UmFrameWork.h"

class DamageSystem : public Component
{
    USING_PROPERTY(DamageSystem)      
    inline static DamageSystem* static_instance = nullptr;
public:
    inline static DamageSystem* GetInstance() 
    {
        if (nullptr == static_instance)
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, u8"Damage System이 존재하지 않습니다. GM에 추가해주세요.");
        }
        return static_instance;
    }


    DamageSystem() = default;
    ~DamageSystem() override = default;

protected:
    REFLECT_PROPERTY()

    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(DamageSystem)

    void Reset() override;

};