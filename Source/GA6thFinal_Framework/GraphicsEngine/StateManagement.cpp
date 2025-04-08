#include "pch.h"
#include "StateManagement.h"

void StateManagement::Initialize()
{
	D3D11_RENDER_TARGET_BLEND_DESC enableRTBD
	{
		.BlendEnable = TRUE,
		.SrcBlend = D3D11_BLEND_SRC_ALPHA,
		.DestBlend = D3D11_BLEND_INV_SRC_ALPHA,
		.BlendOp = D3D11_BLEND_OP_ADD,
		.SrcBlendAlpha = D3D11_BLEND_ONE,
		.DestBlendAlpha = D3D11_BLEND_ZERO,
		.BlendOpAlpha = D3D11_BLEND_OP_ADD,
		.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL,
	};

	D3D11_RENDER_TARGET_BLEND_DESC disableRTBD
	{
		.BlendEnable = FALSE,
		.SrcBlend = D3D11_BLEND_SRC_ALPHA,
		.DestBlend = D3D11_BLEND_INV_SRC_ALPHA,
		.BlendOp = D3D11_BLEND_OP_ADD,
		.SrcBlendAlpha = D3D11_BLEND_ONE,
		.DestBlendAlpha = D3D11_BLEND_ZERO,
		.BlendOpAlpha = D3D11_BLEND_OP_ADD,
		.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL
	};

	_alphaBlendDesc.IndependentBlendEnable = TRUE;
	_alphaBlendDesc.RenderTarget[0] = enableRTBD;
	_alphaBlendDesc.RenderTarget[1] = enableRTBD;
	_alphaBlendDesc.RenderTarget[2] = disableRTBD;
	_alphaBlendDesc.RenderTarget[3] = disableRTBD;
	
	_noneBlendDesc.IndependentBlendEnable = FALSE;
	_noneBlendDesc.RenderTarget[0] = disableRTBD;
}

void StateManagement::AddBlendState(const wchar_t* name, Type type)
{
	ID3D11Device* pDevice = g_pGraphicDevice->GetDevice();
	ID3D11BlendState* pBlendState = nullptr;

	switch (type)
	{
	case StateManagement::Type::NoneBlend:
		pDevice->CreateBlendState(&_noneBlendDesc, &pBlendState);
		break;
	case StateManagement::Type::AlphaBlend:
		pDevice->CreateBlendState(&_alphaBlendDesc, &pBlendState);
		break;
	}

	_blendStates[name] = pBlendState;
}

void StateManagement::Free()
{
	for (auto& [name, state] : _blendStates)
		SafeRelease(state);
}
