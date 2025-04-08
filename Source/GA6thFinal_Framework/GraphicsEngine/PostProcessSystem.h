#pragma once
#include "Base.h"
#include "includes/IPostProcessSystem.h"

class Filter;
class ToneMapping;
class PixelShader;
class PostProcessSystem : public Base, public GE::IPostProcessSystem
{
public:
	explicit PostProcessSystem() = default;
	virtual ~PostProcessSystem() = default;

public:
	// IPostProcessSystem��(��) ���� ��ӵ�
	void GetFilter(GE::IFilter** ppFilter, GE::FilterType type) override;

public:
	std::vector<std::pair<unsigned int, Filter*>>& GetFilters() { return _filters; }

public:
	void Initialize();
	void Render();

private:
	void Blending(ID3D11RenderTargetView* pRTV, ID3D11ShaderResourceView* pSRV);

private:
	// Base��(��) ���� ��ӵ�
	void Free() override;

private:
	std::vector<std::pair<unsigned int, Filter*>> _filters;
	std::shared_ptr<PixelShader> _psBlend;
	ID3D11DeviceContext* _pDeviceContext{ nullptr };	
	ToneMapping* _pToneMapping{ nullptr };
	unsigned int _ID;
};