#pragma once
#include <EnemyAction/EnemyActionBase.h>

class EnemyActionSystem 
    : public Component 
{
public:
    USING_PROPERTY(EnemyActionSystem)
    EnemyActionSystem();
    ~EnemyActionSystem() override;
    inline static EnemyActionSystem* _staticInstance;
    inline static EnemyActionSystem* GetInstance() { return _staticInstance; }

private:
    void Reset() override;
    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

public:
    const EnemyAction::ActionData* GetEnemyActionDataFromID(int actionID);

private:
    REFLECT_FIELDS_BEGIN(Component)
    std::unordered_map<int, std::string> ActionSerializeDataTable;
    REFLECT_FIELDS_END(EnemyActionSystem)
    std::unordered_map<int, EnemyAction::ActionData*> _enemyActionTable; // Action ID와 이름을 매핑하는 테이블
};

// 스킬 ID, 이름, 타입, 