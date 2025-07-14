#pragma once
#include "UmFrameWork.h"
#include <DamageSystem/Interface/IDamageAction.h>

// RevelationActionBase를 상속받은 클래스를 팩토리에 등록할때 사용합니다.
#define REGISTER_REVELATION_CLASS(REVELATION) REGISTER_CLASS(RevelationSystem, REVELATION)

/*
* 계시 효과를 정의하는 Base 클래스 입니다. cpp에서 REGISTER_REVELATION_CLASS를 해야합니다.
* 생성자에서 계시 이름을 지정해줘야 합니다.
*/
class RevelationActionBase abstract : public ReflectSerializer, public IDamageAction
{
    USING_PROPERTY(RevelationActionBase)
public:
    RevelationActionBase(std::u8string_view name) { _name = (const char*)name.data(); }
    virtual ~RevelationActionBase() = default;

    REFLECT_PROPERTY(Name)

    GETTER_ONLY(std::string_view, Name) { return _name; }
    //계시 이름
    PROPERTY(Name)

    /*Action의 효과를 정의하는 내용을 반환해야합니다.*/
    virtual std::string_view GetActionInfo() = 0;

    /*편집을 위한 ImGui 함수를 구현해야합니다.*/
    virtual void ImGuiDrawActionEditor() = 0;

protected:
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    REFLECT_FIELDS_END(RevelationActionBase)

private:
    std::string_view _name;

protected:
    void Execute(CharacterBase* attacker, CharacterBase* target) override;

};

