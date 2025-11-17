#pragma once
#include "TurnSystem/TurnAction/TurnAction.h"

// 반복 전투시 추가되는 소멸 계시를 제어합니다.
class ExtinctionRevelationApply : public TurnAction
{
public:
    ExtinctionRevelationApply();
    ~ExtinctionRevelationApply() override;

protected:
    REFLECT_FIELDS_BEGIN(TurnAction)
    int PlusCount = 0;
    REFLECT_FIELDS_END(ExtinctionRevelationApply)

    const std::string& GetActionName() override;
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;
    void               DeserializedReflectEvent() override;

    void OnRandomExtinctionPushPlayer(size_t& count) override;


private:
    std::string _actionInfo;
    void        UpdateActionInfo();
};