#pragma once
#include <EnemyAction/ActionData/EnemyActionData.h>
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
    const EnemyActionData& GetEnemyActionDataFromID(int actionID);

private:
    REFLECT_FIELDS_BEGIN(Component)
    std::vector<std::string> ActionTableSerialData;
    REFLECT_FIELDS_END(EnemyActionSystem)
    std::unordered_map<int, EnemyActionData> _enemyActionTable; // Action ID와 이름을 매핑하는 테이블

    EnemyActionData& GetEmptyData() { static EnemyActionData emptyData; return emptyData; }
};

// 스킬 ID, 이름, 타입, 