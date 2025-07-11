#pragma once
#include "UmFrameWork.h"

// RevelationActionBase를 상속받은 클래스를 팩토리에 등록할때 사용합니다.
#define REGISTER_REVELATION_CLASS(REVELATION) REGISTER_CLASS(RevelationSystem, REVELATION)

/*
* 계시 효과를 정의하는 Base 클래스 입니다. cpp에서 REGISTER_REVELATION_CLASS를 해야합니다.
*/
class RevelationActionBase abstract : public ReflectSerializer
{
    USING_PROPERTY(RevelationActionBase)
public:
    RevelationActionBase() = default;
    virtual ~RevelationActionBase() = default;

    REFLECT_PROPERTY()
protected:
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    REFLECT_FIELDS_END(RevelationActionBase)

};

