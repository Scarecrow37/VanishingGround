#pragma once
#include "Base.h"

struct RenderTargetGroup
{
	std::vector<ID3D11RenderTargetView*>	renderTargets;
	std::vector<ID3D11ShaderResourceView*>	textures;
};

struct View
{
	ID3D11View*					view;
	ID3D11ShaderResourceView*	texture;
};

class ViewManagement : public Base
{
public:
	explicit ViewManagement() = default;
	virtual ~ViewManagement() = default;

public:
	ID3D11RenderTargetView* GetRenderTargetView(const wchar_t* name);
	ID3D11DepthStencilView* GetDepthStencilView(const wchar_t* name);
	ID3D11ShaderResourceView* GetShaderResourceView(const wchar_t* name);

public:
	// RTV
	RenderTargetGroup& GetRenderTargetGroup(const wchar_t* group);
	void AddRenderTargetGroup(const wchar_t* group, const wchar_t* name);
	void AddRenderTargetView(const wchar_t* name, const Vector2& size, DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT);
	void AddMipMapRenderTargetView(const wchar_t* name, const Vector2& size, DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT);

public:
	// DSV
	void AddDepthStencilView(const wchar_t* name, const D3D11_TEXTURE2D_DESC& textureDesc, const D3D11_DEPTH_STENCIL_VIEW_DESC& viewDesc);

private:
	// Base을(를) 통해 상속됨
	void Free() override;

private:
	std::unordered_map<std::wstring, View> _views;
	std::unordered_map<std::wstring, RenderTargetGroup> _rtvViews;
};