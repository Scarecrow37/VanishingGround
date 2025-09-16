#pragma once
#include "DLLExportDefine.h"
#include "Utility/SingletonHelper.h"

class ConsumableSystem : public Component
{
    USING_PROPERTY(ConsumableSystem)

public:
    ConsumableSystem();
    ~ConsumableSystem() override;

private:
    SingletonComponent<ConsumableSystem> _singletonComponent{this};

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(ConsumableSystem)

    void Reset() override;
    void Awake() override;
    void ImGuiDrawPropertysEvent() override;
    
};
