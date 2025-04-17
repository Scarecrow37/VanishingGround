#include "pch.h"
#include "ViewManager.h"

ViewManager::ViewManager()
	: _shaderResourceHeap(nullptr)
	, _renderTargetHeap(nullptr)
	, _depthStencilHeap(nullptr)
	, _shaderResourceDescriptorSize(0)
	, _renderTargetDescriptorSize(0)
	, _depthStencilDescriptorSize(0)
	, _numShaderResource(0)
	, _numRenderTarget(0)
	, _numDepthStencil(0)
{	
}

HRESULT ViewManager::Initialize()
{
	HRESULT hr = S_OK;
	ComPtr<ID3D12Device> device = UmDevice.GetDevice();
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};

	desc.NumDescriptors = 1000;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	
	hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(_shaderResourceHeap.GetAddressOf()));
	FAILED_CHECK_BREAK(hr);

	desc.NumDescriptors = 100;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	
	hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(_renderTargetHeap.GetAddressOf()));
	FAILED_CHECK_BREAK(hr);

	desc.NumDescriptors = 10;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	
	hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(_depthStencilHeap.GetAddressOf()));
	FAILED_CHECK_BREAK(hr);

	_shaderResourceDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	_renderTargetDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	_depthStencilDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	return hr;
}

HRESULT ViewManager::AddDescriptorHeap(const ViewManager::Type type, D3D12_CPU_DESCRIPTOR_HANDLE& handle)
{
	HRESULT hr = S_OK;
	UINT offset = 0;

	switch (type)
	{
	case ViewManager::Type::SHADER_RESOURCE:
		offset = _shaderResourceDescriptorSize * _numShaderResource;
		handle = _shaderResourceHeap->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += offset;
		_numShaderResource++;
		break;

	case ViewManager::Type::RENDER_TARGET:
		offset = _renderTargetDescriptorSize * _numRenderTarget;
		handle = _renderTargetHeap->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += offset;
		_numRenderTarget++;
		break;

	case ViewManager::Type::DEPTH_STENCIL:
		offset = _depthStencilDescriptorSize * _numDepthStencil;
		handle = _depthStencilHeap->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += offset;
		_numDepthStencil++;
		break;

	default:
		hr = E_FAIL;
		__debugbreak();
		break;
	}

	return hr;
}

HRESULT ViewManager::AddDescriptorHeap(const ViewManager::Type type, UINT numDescriptors, std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& handles)
{
	HRESULT hr = S_OK;
	UINT offset = 0;
	handles.resize(numDescriptors);

	for (auto& handle : handles)
	{
		switch (type)
		{
		case ViewManager::Type::SHADER_RESOURCE:
			offset = _shaderResourceDescriptorSize * _numShaderResource;
			handle = _shaderResourceHeap->GetCPUDescriptorHandleForHeapStart();
			handle.ptr += offset;
			_numShaderResource++;
			break;

		case ViewManager::Type::RENDER_TARGET:
			offset = _renderTargetDescriptorSize * _numRenderTarget;
			handle = _renderTargetHeap->GetCPUDescriptorHandleForHeapStart();
			handle.ptr += offset;
			_numRenderTarget++;
			break;

		case ViewManager::Type::DEPTH_STENCIL:
			offset = _depthStencilDescriptorSize * _numDepthStencil;
			handle = _depthStencilHeap->GetCPUDescriptorHandleForHeapStart();
			handle.ptr += offset;
			_numDepthStencil++;
			break;

		default:
			hr = E_FAIL;
			__debugbreak();
			break;
		}
	}

	return hr;
}	