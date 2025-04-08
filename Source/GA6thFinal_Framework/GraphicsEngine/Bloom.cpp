#include "pch.h"
#include "Bloom.h"
#include "PixelShader.h"

void Bloom::Initialize()
{
	_psBloomCurve = g_pResourceMgr->LoadResource<PixelShader>(L"Assets/Shaders/BloomCurvePS.cso");
	_psSampling = g_pResourceMgr->LoadResource<PixelShader>(L"Assets/Shaders/SamplingPS.cso");

	_desc =
	{
		.weight = 0.3f,
		.threshold = 1.f,
		.level = 5
	};
}

void Bloom::Release()
{
	delete this;
}

void Bloom::GetDesc(GE::BlOOM_DESC* pOutDesc)
{
	(*pOutDesc) = _desc;
}

void Bloom::SetDesc(GE::BlOOM_DESC* pInDesc)
{
	_desc = (*pInDesc);
}

void Bloom::Render()
{
	auto* pRTV = g_pViewManagement->GetRenderTargetView(L"MipMap");
	auto* pSRV = g_pViewManagement->GetShaderResourceView(L"MipMap");

	_pDeviceContext->OMSetRenderTargets(1, &pRTV, nullptr);
	_pDeviceContext->ClearRenderTargetView(pRTV, COLOR_ZERO);
	_psBloomCurve->SetPixelShader();
	g_pQuad->Render();
	
	_pDeviceContext->GenerateMips(pSRV);
	_pDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

	pRTV = g_pViewManagement->GetRenderTargetView(L"PostProcess");

	_pDeviceContext->OMSetRenderTargets(1, &pRTV, nullptr);
	_pDeviceContext->PSSetShaderResources(2, 1, &pSRV);
	_psSampling->SetPixelShader();
	g_pQuad->Render();

	ClearBindResource(_pDeviceContext, 2, 1);
}