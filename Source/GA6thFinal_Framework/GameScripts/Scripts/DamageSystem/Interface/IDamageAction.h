#pragma once
class CharacterBase;

/*데미지 시스템에 의해 데미지를 줄때 호출되는 Action 인터페이스 입니다.*/
class IDamageAction abstract
{
public:
    IDamageAction() = default;
    virtual ~IDamageAction() = default;

    virtual void Execute(CharacterBase* attacker, CharacterBase* target) = 0;
};