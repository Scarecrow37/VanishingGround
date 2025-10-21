#pragma once
#include "IFSMEntity.h"

class FSMCondition : public IFSMEntity
{
    USING_PROPERTY(FSMCondition)
public:
    FSMCondition() = default;
    virtual ~FSMCondition() override = default;

    /// <summary>
    /// 전이 조건 검사 함수
    /// </summary>
    /// <returns></returns>
    virtual bool Evaluate() = 0;

    GETTER(int, Order) { return ReflectFields->Order; }
    SETTER(int, Order) { ReflectFields->Order = ReflectFields->Order; }
    PROPERTY(Order)

    REFLECT_FIELDS_BEGIN(IFSMEntity)
    //전이 조건의 우선순위를 결정합니다. 숫자가 낮을수록 우선순위가 높습니다.
    int Order = 0;
    REFLECT_FIELDS_END(FSMCondition)

};

template <typename T>
concept FSM_CONDITION_BASE = std::is_base_of_v<FSMCondition, T>;