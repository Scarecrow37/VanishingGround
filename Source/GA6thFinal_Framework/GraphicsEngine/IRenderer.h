#pragma once
#include "Base.h"

class IRenderer : public Base
{
public:
	virtual void Initialize() = 0;
	virtual void Render() = 0;
};