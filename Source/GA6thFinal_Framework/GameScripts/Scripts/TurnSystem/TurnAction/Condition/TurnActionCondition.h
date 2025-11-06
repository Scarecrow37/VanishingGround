#pragma once
#include "UmFramework.h"
#include <TurnSystem/TurnAction/TurnActionEnums.h>

class CharacterBase;
    //턴 액션 조건 재사용을 위한 Base 클래스
class TurnActionCondition : public ReflectSerializer
{
    USING_PROPERTY(TurnActionCondition)
public:
    TurnActionCondition() = default;
    ~TurnActionCondition() override = default;

    /// <summary>
    /// 조건 검사 함수
    /// </summary>
    /// <returns></returns>
    virtual bool Evaluate() = 0;

    /// <summary>
    /// 에디터용 ImGui Draw를 요청합니다. 
    /// </summary>
    virtual void DrawImguiEditor() = 0;

    /// <summary>
    /// Condition을 설명하는 문자열 반환합니다.
    /// </summary>
    virtual const std::string& GetConditionInfo() = 0;

    /// <summary>
    /// Owner Action의 EvaluateConditions가 계산된 이후 결과를 알려주는 이벤트입니다.
    /// </summary>
    virtual void OnEvaluateConditions(bool result) {}

    virtual bool CheckEvaluate(CharacterBase* character) = 0;

protected:
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    REFLECT_FIELDS_END(TurnActionCondition)

};