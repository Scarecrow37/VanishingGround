#pragma once
#include "Base.h"
#include "Filter.h"

class ToneMapping : public Base, public Filter
{
public:
	explicit ToneMapping() = default;
	virtual ~ToneMapping() = default;

public:
	// Filter을(를) 통해 상속됨
	void Initialize() override;
	void Render() override;

private:
	// Base을(를) 통해 상속됨
	void Free() override;

private:
	std::shared_ptr<PixelShader> _psToneMapping;
	std::shared_ptr<PixelShader> _psLuminance;
};

