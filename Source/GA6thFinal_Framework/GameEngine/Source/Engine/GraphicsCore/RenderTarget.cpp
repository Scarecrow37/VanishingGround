#include "pch.h"
#include "RenderTarget.h"

HRESULT RenderTarget::Initialize()
{
	ComPtr<ID3D12Device> device = UmDevice.GetDevice();

	HRESULT hr = S_OK;

	hr = UmViewManager.AddDescriptorHeap(ViewManager::Type::RENDER_TARGET, _handle);
	FAILED_CHECK_BREAK(hr);
	
	return hr;	
}