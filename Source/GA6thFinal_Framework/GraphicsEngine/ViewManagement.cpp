#include "pch.h"
#include "ViewManagement.h"

ID3D11RenderTargetView* ViewManagement::GetRenderTargetView(const wchar_t* name)
{
	assert(_views.find(name) != _views.end());
	return static_cast<ID3D11RenderTargetView*>(_views[name].view);
}

ID3D11DepthStencilView* ViewManagement::GetDepthStencilView(const wchar_t* name)
{
	assert(_views.find(name) != _views.end());
	return static_cast<ID3D11DepthStencilView*>(_views[name].view);
}

ID3D11ShaderResourceView* ViewManagement::GetShaderResourceView(const wchar_t* name)
{
	assert(_views.find(name) != _views.end());
	return _views[name].texture;
}

RenderTargetGroup& ViewManagement::GetRenderTargetGroup(const wchar_t* group)
{
	assert(_rtvViews.find(group) != _rtvViews.end());
	return _rtvViews[group];
}

void ViewManagement::AddRenderTargetGroup(const wchar_t* group, const wchar_t* name)
{
	assert(_views.find(name) != _views.end());

	auto& [renderTargets, textures] = _rtvViews[group];
	auto& [pRTV, pSRV]				= _views[name];

	renderTargets.push_back(static_cast<ID3D11RenderTargetView*>(pRTV));
	textures.push_back(pSRV);
}

void ViewManagement::AddRenderTargetView(const wchar_t* name, const Vector2& size, DXGI_FORMAT format)
{
	ID3D11Device* pDevice = g_pGraphicDevice->GetDevice();

	ID3D11Texture2D* pTexture = nullptr;
	D3D11_TEXTURE2D_DESC textureDesc
	{
		.Width = (unsigned int)size.x,
		.Height = (unsigned int)size.y,
		.MipLevels = 1,
		.ArraySize = 1,
		.Format = format,
		.SampleDesc {.Count = 1 },
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE
	};

	pDevice->CreateTexture2D(&textureDesc, nullptr, &pTexture);

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc
	{
		.Format = textureDesc.Format,
		.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D
	};

	ID3D11RenderTargetView* pRTV = nullptr;
	pDevice->CreateRenderTargetView(pTexture, &rtvDesc, &pRTV);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc
	{
		.Format = textureDesc.Format,
		.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
		.Texture2D
		{
			.MostDetailedMip = 0,
			.MipLevels = 1
		}
	};

	ID3D11ShaderResourceView* pSRV = nullptr;
	pDevice->CreateShaderResourceView(pTexture, &srvDesc, &pSRV);
	SafeRelease(pTexture);

	_views[name] = { pRTV, pSRV };
}

void ViewManagement::AddMipMapRenderTargetView(const wchar_t* name, const Vector2& size, DXGI_FORMAT format)
{
	ID3D11Device* pDevice = g_pGraphicDevice->GetDevice();

	ID3D11Texture2D* pTexture = nullptr;
	D3D11_TEXTURE2D_DESC textureDesc
	{
		.Width = (unsigned int)size.x,
		.Height = (unsigned int)size.y,
		.MipLevels = 0,
		.ArraySize = 1,
		.Format = format,
		.SampleDesc {.Count = 1 },
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS
	};

	pDevice->CreateTexture2D(&textureDesc, nullptr, &pTexture);

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc
	{
		.Format = textureDesc.Format,
		.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D
	};

	ID3D11RenderTargetView* pRTV = nullptr;
	pDevice->CreateRenderTargetView(pTexture, &rtvDesc, &pRTV);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc
	{
		.Format = textureDesc.Format,
		.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
		.Texture2D
		{
			.MostDetailedMip = 0,
			.MipLevels = (UINT)-1
		}
	};

	ID3D11ShaderResourceView* pSRV = nullptr;
	pDevice->CreateShaderResourceView(pTexture, nullptr, &pSRV);
	SafeRelease(pTexture);

	_views[name] = { pRTV, pSRV };
}

void ViewManagement::AddDepthStencilView(const wchar_t* name, const D3D11_TEXTURE2D_DESC& textureDesc, const D3D11_DEPTH_STENCIL_VIEW_DESC& viewDesc)
{
	assert(_views.find(name) == _views.end());

	ID3D11Device* pDevice = g_pGraphicDevice->GetDevice();	

	ID3D11Texture2D* pTexture = nullptr;
	pDevice->CreateTexture2D(&textureDesc, nullptr, &pTexture);

	ID3D11DepthStencilView* pDSV = nullptr;
	pDevice->CreateDepthStencilView(pTexture, &viewDesc, &pDSV);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc
	{
		.Format = DXGI_FORMAT_R32_FLOAT,
		.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
		.Texture2D {.MipLevels = 1 },
	};

	ID3D11ShaderResourceView* pSRV = nullptr;
	if (textureDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		pDevice->CreateShaderResourceView(pTexture, &srvDesc, &pSRV);
	}
	SafeRelease(pTexture);

	_views[name] = { pDSV, pSRV };
}

void ViewManagement::Free()
{
	for (auto& group : _views)
	{
		auto& [view, texture] = group.second;

		SafeRelease(view);
		SafeRelease(texture);
	}
}