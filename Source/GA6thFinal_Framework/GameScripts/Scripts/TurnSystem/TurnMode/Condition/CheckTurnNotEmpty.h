#pragma once
#include "CheckTurnEmpty.h"

class CheckTurnNotEmpty : public CheckTurnEmpty
{
public:
   virtual bool Evaluate() override;
};