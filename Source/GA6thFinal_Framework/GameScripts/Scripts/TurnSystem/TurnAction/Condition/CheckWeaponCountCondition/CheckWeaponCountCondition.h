#pragma once
#include <TurnSystem/TurnAction/TurnAction.h>

// 플레이어의 무기 순서를 확인하는 조건 입니다.
class CheckWeaponCountCondition : public TurnActionCondition
{
    friend class TurnMode;
    USING_PROPERTY(CheckWeaponCountCondition)
public:
    CheckWeaponCountCondition() { UpdateInfo(); }
    ~CheckWeaponCountCondition() override = default;

    REFLECT_PROPERTY(TurnCount)

    GETTER(int, TurnCount) { return ReflectFields->TurnCount; }
    SETTER(int, TurnCount) { SetTurnCount(value); }
    PROPERTY(TurnCount)
    void SetTurnCount(int value);

protected:
    REFLECT_FIELDS_BEGIN(TurnActionCondition)
    int TurnCount = 1;
    REFLECT_FIELDS_END(CheckWeaponCountCondition)

    void DeserializedReflectEvent() override;

    bool               Evaluate() override;
    void               DrawImguiEditor() override;
    const std::string& GetConditionInfo() override;

private:
    void        UpdateInfo();
    std::string _conditionInfo;

private:
    bool CheckEvaluate(CharacterBase* character) override;
};
