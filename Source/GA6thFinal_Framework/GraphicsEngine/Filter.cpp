#include "pch.h"
#include "Filter.h"

Filter::Filter()
{
	_pDeviceContext = g_pGraphicDevice->GetDeviceContext();

	auto& [RTVs, SRVs] = g_pViewManagement->GetRenderTargetGroup(L"Blend2");

	for (int i = 0; i < 2; i++)
	{
		_pBlendRTVs[i] = RTVs[i];
		_pBlendSRVs[i] = SRVs[i];
	}

	_pPostRTV = g_pViewManagement->GetRenderTargetView(L"PostProcess");
	_pPostSRV = g_pViewManagement->GetShaderResourceView(L"PostProcess");
}