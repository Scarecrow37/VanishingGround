#include "CharacterBase.h"
CharacterBase::CharacterBase() : 
    _hp(0), 
    _mp(0) 
{

}
CharacterBase::~CharacterBase() = default;

void CharacterBase::Awake() 
{
    Base::Awake();
    gameObject->AddTag(TAG);
}
