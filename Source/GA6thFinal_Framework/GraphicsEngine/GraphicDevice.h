#pragma once
#include "Base.h"

class GraphicDevice : public Base
{
public:
	explicit GraphicDevice() = default;
	virtual ~GraphicDevice() = default;

public:
	ID3D11DeviceContext* GetDeviceContext() const { return _pDeviceContext; }
	ID3D11Device* GetDevice() const { return _pDevice; }
	IDXGISwapChain* GetSwapChain() const { return _pSwapChain; }
	ID3D11RenderTargetView* GetBackBuffer() const { return _pBackBuffer; }

public:
	HRESULT Initialize(HWND hWnd, bool isFullScreen);

private:
	HRESULT CreateSwapChain(HWND hWnd, bool isFullScreen);

private:
	// Base을(를) 통해 상속됨
	void Free() override;

private:
	ID3D11Device*			_pDevice{ nullptr };
	ID3D11DeviceContext*	_pDeviceContext{ nullptr };
	IDXGISwapChain*			_pSwapChain{ nullptr };
	ID3D11RenderTargetView* _pBackBuffer{ nullptr };
};