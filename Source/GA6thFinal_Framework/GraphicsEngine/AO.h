#pragma once
#include "Base.h"
#include "Filter.h"

class AO abstract : public Base, public Filter
{
public:
	virtual ID3D11ShaderResourceView* GetTexture() = 0;
};