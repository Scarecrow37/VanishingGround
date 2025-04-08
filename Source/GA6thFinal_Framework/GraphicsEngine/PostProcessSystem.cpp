#include "pch.h"
#include "PostProcessSystem.h"
#include "PixelShader.h"

// Filters
#include "Bloom.h"
#include "Blur.h"
#include "ToneMapping.h"

void PostProcessSystem::GetFilter(GE::IFilter** ppFilter, GE::FilterType type)
{
	(*ppFilter) = reinterpret_cast<GE::IFilter*>(_filters[(int)type].second);
}

void PostProcessSystem::Initialize()
{	
	_pDeviceContext = g_pGraphicDevice->GetDeviceContext();	

	_filters.resize((size_t)GE::FilterType::End);

	_pToneMapping = new ToneMapping;
	_pToneMapping->Initialize();

	Bloom* pBloom = new Bloom;
	pBloom->Initialize();
	_filters[(int)GE::FilterType::Bloom] = { 1 << _ID++, pBloom };

	Blur* pBlur = new Blur;
	pBlur->Initialize();
	_filters[(int)GE::FilterType::Blur] = { 1 << _ID++, pBlur };

	Vector2 screenSize{ g_width, g_height };
	g_pConstantBuffer->UpdateConstantBuffer(L"ScreenSize", &screenSize);	

	_psBlend = g_pResourceMgr->LoadResource<PixelShader>(L"Assets/Shaders/BlendPS.cso");
}

void PostProcessSystem::Render()
{
	_pDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

	auto& [RTVs, SRVs] = g_pViewManagement->GetRenderTargetGroup(L"Blend");
	auto* pBaseSRV = g_pViewManagement->GetShaderResourceView(L"Base");
	auto* pLayerMaskSRV = g_pViewManagement->GetShaderResourceView(L"LayerMask");
	auto* pPostProcessRTV = g_pViewManagement->GetRenderTargetView(L"PostProcess");

	_pDeviceContext->PSSetShaderResources(0, 1, &pBaseSRV);
	_pDeviceContext->PSSetShaderResources(1, 1, &pLayerMaskSRV);
	_pDeviceContext->ClearRenderTargetView(pPostProcessRTV, COLOR_ZERO);

	unsigned int currentIndex = 0;
	ID3D11ShaderResourceView* pSRV = nullptr;

	for (auto& [mask, filter] : _filters)
	{
		g_pConstantBuffer->UpdateConstantBuffer(L"LayerMask", &mask);

		filter->Render();
		Blending(RTVs[currentIndex], pSRV);
		pSRV = SRVs[currentIndex];
		currentIndex = (currentIndex + 1) % 2;
	}

	// Blending
	ClearBindResource(_pDeviceContext, 2, 2);
	auto* pPostProcessTarget = g_pViewManagement->GetRenderTargetView(L"PostProcess");

	_pDeviceContext->OMSetRenderTargets(1, &pPostProcessTarget, nullptr);
	_pDeviceContext->PSSetShaderResources(2, 1, &pBaseSRV);
	_pDeviceContext->PSSetShaderResources(3, 1, &SRVs[currentIndex]);
	_psBlend->SetPixelShader();
	g_pQuad->Render();

	// ToneMapping
	_pToneMapping->Render();
}

void PostProcessSystem::Blending(ID3D11RenderTargetView* pRTV, ID3D11ShaderResourceView* pSRV)
{
	auto* pPostSRV = g_pViewManagement->GetShaderResourceView(L"PostProcess");
	ClearBindResource(_pDeviceContext, 2, 1);
	_pDeviceContext->OMSetRenderTargets(1, &pRTV, nullptr);
	_pDeviceContext->PSSetShaderResources(2, 1, &pSRV);
	_pDeviceContext->PSSetShaderResources(3, 1, &pPostSRV);
	_psBlend->SetPixelShader();
	g_pQuad->Render();
	ClearBindResource(_pDeviceContext, 3, 1);
}

void PostProcessSystem::Free()
{
	for (auto& [ID, filter] : _filters)
	{
		delete filter;
	}

	SafeRelease(_pToneMapping);
}