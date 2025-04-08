#include "pch.h"
#include "Blur.h"
#include "PixelShader.h"

void Blur::Release()
{
	delete this;
}

void Blur::GetDesc(GE::BLUR_DESC* pOutDesc)
{
}

void Blur::SetDesc(GE::BLUR_DESC* pInDesc)
{
}

void Blur::Initialize()
{
	/*_psGaussianBlur[X] = g_pResourceMgr->LoadResource<PixelShader>(L"Assets/Shaders/GaussianBlurPS_X.cso");
	_psGaussianBlur[Y] = g_pResourceMgr->LoadResource<PixelShader>(L"Assets/Shaders/GaussianBlurPS_Y.cso");*/
}

void Blur::Render()
{
	//auto* pRTV = g_pViewManagement->GetRenderTargetView(L"PostProcess");
	//auto* pSRV = g_pViewManagement->GetShaderResourceView(L"PostProcess");
	//_pDeviceContext->ClearRenderTargetView(pRTV, COLOR_ZERO);

 //   // X Blur
 //   _pDeviceContext->OMSetRenderTargets(1, &_pBlendRTVs[0], nullptr);
 //   _pDeviceContext->PSSetShaderResources(2, 1, &_pPostSRV);
 //   _psGaussianBlur[X]->SetPixelShader();
 //   g_pQuad->Render();
 //   ClearBindResource(_pDeviceContext, 2, 1);

 //   // Y Blur
 //   _pDeviceContext->OMSetRenderTargets(1, &_pBlendRTVs[1], nullptr);
 //   _pDeviceContext->PSSetShaderResources(2, 1, &_pBlendSRVs[0]);
 //   _psGaussianBlur[Y]->SetPixelShader();
 //   g_pQuad->Render();
 //   ClearBindResource(_pDeviceContext, 2, 1);
}
