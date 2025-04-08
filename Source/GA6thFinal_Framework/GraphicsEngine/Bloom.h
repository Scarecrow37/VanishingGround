#pragma once
#include "Filter.h"
#include "includes/IBloom.h"

class Bloom : public Filter, public GE::IBloom
{
public:
	explicit Bloom() = default;
	virtual ~Bloom() = default;

public:
	void Initialize() override;

public:
	// IGaussianBlur을(를) 통해 상속됨
	void Release() override;
	void GetDesc(GE::BlOOM_DESC* pOutDesc) override;
	void SetDesc(GE::BlOOM_DESC* pInDesc) override;

public:
	// PostProcess을(를) 통해 상속됨
	void Render() override;

private:
	std::shared_ptr<PixelShader> _psBloomCurve;
	std::shared_ptr<PixelShader> _psSampling;
	GE::BlOOM_DESC _desc;
};