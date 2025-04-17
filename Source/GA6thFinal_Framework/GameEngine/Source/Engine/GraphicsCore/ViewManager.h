#pragma once

class ViewManager
{
public:
	enum class Type { SHADER_RESOURCE, RENDER_TARGET, DEPTH_STENCIL };

public:
	ViewManager();
	~ViewManager() = default;

public:
	ComPtr<ID3D12DescriptorHeap> GetShaderResourceHeap() const { return _shaderResourceHeap; }
	ComPtr<ID3D12DescriptorHeap> GetRenderTargetHeap() const { return _renderTargetHeap; }
	ComPtr<ID3D12DescriptorHeap> GetDepthStencilHeap() const { return _depthStencilHeap; }

public:
	HRESULT Initialize();
	HRESULT AddDescriptorHeap(const ViewManager::Type type, D3D12_CPU_DESCRIPTOR_HANDLE& handle);
	HRESULT AddDescriptorHeap(const ViewManager::Type type, UINT numDescriptors, std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& handles);

private:
	ComPtr<ID3D12DescriptorHeap> _shaderResourceHeap;
	ComPtr<ID3D12DescriptorHeap> _renderTargetHeap;
	ComPtr<ID3D12DescriptorHeap> _depthStencilHeap;

	UINT _shaderResourceDescriptorSize;
	UINT _renderTargetDescriptorSize;
	UINT _depthStencilDescriptorSize;
	UINT _numShaderResource;
	UINT _numRenderTarget;
	UINT _numDepthStencil;
};
