#pragma once
#include "UmFrameWork.h"
#include <Interface/ITriggerType.h>

/*
* 계시 효과를 정의하는 Base 클래스 입니다. cpp에서 REGISTER_REVELATION_CLASS를 해야합니다.
*/
class RevelationActionBase abstract : public ReflectSerializer, public ITriggerType
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

