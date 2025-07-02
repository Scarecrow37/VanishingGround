#pragma once
#include "CheckTurnEmpty.h"

/*
* TurnList 항목이 존재하는지 확인하는 조건 입니다.
*/
class CheckTurnNotEmpty : public CheckTurnEmpty
{
public:
   virtual bool Evaluate() override;
};