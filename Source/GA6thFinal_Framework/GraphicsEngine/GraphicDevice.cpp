#include "pch.h"
#include "GraphicDevice.h"

HRESULT GraphicDevice::Initialize(HWND hWnd, bool isFullScreen)
{
	FAILED_CHECK_BREAK(CreateSwapChain(hWnd, isFullScreen));

	return S_OK;
}

HRESULT GraphicDevice::CreateSwapChain(HWND hWnd, bool isFullScreen)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc
	{
		.BufferDesc
		{
			.Width = (unsigned int)g_width,
			.Height = (unsigned int)g_height,
			.RefreshRate
			{
				.Numerator = 60,
				.Denominator = 1
			},
			.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
		},
		.SampleDesc
		{
			.Count = 1,
			.Quality = 0
		},
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = 2,
		.OutputWindow = hWnd,
		.Windowed = !isFullScreen,
		.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
		//.SwapEffect = DXGI_SWAP_EFFECT_DISCARD
		//.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL
	};

	UINT creationFlags = 0;
#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	FAILED_CHECK_BREAK(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, creationFlags, nullptr, 0,
		D3D11_SDK_VERSION, &swapChainDesc, &_pSwapChain, &_pDevice, nullptr, &_pDeviceContext));

	ID3D11Texture2D* pBackBufferTexture = nullptr;
	FAILED_CHECK_BREAK(_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture));
	FAILED_CHECK_BREAK(_pDevice->CreateRenderTargetView(pBackBufferTexture, nullptr, &_pBackBuffer));
	SafeRelease(pBackBufferTexture);

	return S_OK;
}

void GraphicDevice::Free()
{
	SafeRelease(_pBackBuffer);
	SafeRelease(_pSwapChain);
	SafeRelease(_pDeviceContext);
	SafeRelease(_pDevice);
}