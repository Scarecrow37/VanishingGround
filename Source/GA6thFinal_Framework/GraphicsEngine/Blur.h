#pragma once
#include "Filter.h"
#include "includes/IBlur.h"

class Blur : public Filter, public GE::IBlur
{
	enum Type { X, Y, End };

public:
	explicit Blur() = default;
	virtual ~Blur() = default;

public:
	// IBlur��(��) ���� ��ӵ�
	void Release() override;
	void GetDesc(GE::BLUR_DESC* pOutDesc) override;
	void SetDesc(GE::BLUR_DESC* pInDesc) override;

public:
	// Filter��(��) ���� ��ӵ�
	void Initialize() override;
	void Render() override;

private:
	std::shared_ptr<PixelShader> _psGaussianBlur[End];
	GE::BLUR_DESC _desc;
};