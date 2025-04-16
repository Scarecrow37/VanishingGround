#include "pch.h"
#include "Device.h"

float Device::GetEngineTime()
{
	static ULONGLONG oldtime = GetTickCount64();
	ULONGLONG 		 nowtime = GetTickCount64();
	float deltaTime = (nowtime - oldtime) * 0.001f;
	oldtime = nowtime;

	return deltaTime;
}

void Device::SetUpDevice(HWND hwnd, UINT width, UINT height, FEATURE_LEVEL feature)
{
	_mode.Width = width;
	_mode.Height = height;
#ifndef NDEBUG
	ComPtr<ID3D12Debug> debugController;
	FAILED_CHECK_BREAK(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf())));
	debugController->EnableDebugLayer();
#endif // !NDEBUG
	D3D_FEATURE_LEVEL d3dFeature{};

	switch (feature)
	{
	case FEATURE_LEVEL::LEVEL_11_0:
		d3dFeature = D3D_FEATURE_LEVEL_11_0;
		break;
	case FEATURE_LEVEL::LEVEL_12_0:
		d3dFeature = D3D_FEATURE_LEVEL_12_0;
		break;
	case FEATURE_LEVEL::LEVEL_12_1:
		d3dFeature = D3D_FEATURE_LEVEL_12_1;
		break;
	default:
		break;
	}

	CreateDeviceAndSwapChain(hwnd, d3dFeature);
}

void Device::Initialize()
{
	HRESULT hr = S_OK;

	hr = UmViewManager.AddDescriptorHeap(ViewManager::Type::RENDER_TARGET, SWAPCHAIN_BUFFER_COUNT, _renderTargetHandles);
	FAILED_CHECK_BREAK(hr);

	hr = UmViewManager.AddDescriptorHeap(ViewManager::Type::DEPTH_STENCIL, _depthStencilHandle);
	FAILED_CHECK_BREAK(hr);

	OnResize(_mode.Width, _mode.Height);
}

void Device::Finalize()
{
	CloseHandle(_fenceEvent);
}

void Device::OnResize(UINT width, UINT height)
{
	_mode.Width = width;
	_mode.Height = height;
	assert(_device);
	assert(_swapChain);
	GPUSync();
	FAILED_CHECK_BREAK(_commandList->Reset(_commandAllocator.Get(),nullptr));
	CreateRenderTarget();
	CreateDepthStencil();
	
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(_depthStencilBuffer.Get(),
														D3D12_RESOURCE_STATE_COMMON, 
														D3D12_RESOURCE_STATE_DEPTH_WRITE);

	_commandList->ResourceBarrier(1, &barrier);
	FAILED_CHECK_BREAK(_commandList->Close());
	RegisterCommand(_commandList.Get());
	ExecuteCommand();
	GPUSync();
	_mainViewport.TopLeftX = 0;
	_mainViewport.TopLeftY = 0;
	_mainViewport.Width = static_cast<float>(_mode.Width);
	_mainViewport.Height = static_cast<float>(_mode.Height);
	_mainViewport.MinDepth = 0.0f;
	_mainViewport.MaxDepth = 1.0f;

	_mainrRect = { 0, 0, static_cast<long>(_mode.Width), static_cast<long>(_mode.Height) };
}

void Device::SetViewPort()
{
	D3D12_VIEWPORT viewPort
	{
		.TopLeftX = 0,
		.TopLeftY = 0,
		.Width = (FLOAT)_mode.Width,
		.Height = (FLOAT)_mode.Height,
		.MinDepth = 0.0f,
		.MaxDepth = 1.0f,
	};

	//g_CmdList->RSSetViewports(1, &vp);
	_mainViewport = viewPort;
}

void Device::CreateDeviceAndSwapChain(HWND hwnd, D3D_FEATURE_LEVEL feature)
{
	CreateDXGIFactory2(0, IID_PPV_ARGS(_dxgiFactory.GetAddressOf()));
	FAILED_CHECK_BREAK(CreateDXGIFactory2(0, IID_PPV_ARGS(_dxgiFactory.GetAddressOf())));
	FAILED_CHECK_BREAK(D3D12CreateDevice(0, feature, IID_PPV_ARGS(_device.GetAddressOf())));

	_rtvDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	_dsvDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	_cbvSrvUavDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	// 4X MSAA 품질 수준 지원 점검
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Format = _backBufferFormat;
	msQualityLevels.NumQualityLevels = 0;
	FAILED_CHECK_BREAK(_device->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels,
		sizeof(msQualityLevels)
	));
	_4xMsaaQuality = msQualityLevels.NumQualityLevels;
	assert(_4xMsaaQuality > 0 && "Unexpected MSAA quality level");

	CreateCommandQueue();
	CreateSyncObject();

	_swapChain.Reset();
	DXGI_SWAP_CHAIN_DESC1 sd{};
	sd.Width = _mode.Width;
	sd.Height = _mode.Height;
	sd.Format = _backBufferFormat;
	sd.SampleDesc.Count = _4xMsaaState ? 4 : 1;
	sd.SampleDesc.Quality = _4xMsaaState ? (_4xMsaaQuality - 1) : 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 후면 버퍼의 속성
	sd.BufferCount = SWAPCHAIN_BUFFER_COUNT;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	ComPtr<IDXGISwapChain1> swapChain;
	FAILED_CHECK_BREAK(_dxgiFactory->CreateSwapChainForHwnd(_commandQueue.Get(), hwnd, &sd, nullptr, nullptr, swapChain.GetAddressOf()));
	FAILED_CHECK_BREAK(swapChain->QueryInterface(IID_PPV_ARGS(_swapChain.GetAddressOf())));
	_renderTargetIndex = _swapChain->GetCurrentBackBufferIndex();
}

void Device::CreateCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC desc
	{
		.Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
		.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
		.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
		.NodeMask = 0,
	};
	FAILED_CHECK_BREAK(_device->CreateCommandQueue(&desc, IID_PPV_ARGS(_commandQueue.GetAddressOf())));
	FAILED_CHECK_BREAK(_device->CreateCommandAllocator(desc.Type, IID_PPV_ARGS(_commandAllocator.GetAddressOf())));
	FAILED_CHECK_BREAK(_device->CreateCommandList(desc.NodeMask, desc.Type, _commandAllocator.Get(), nullptr, IID_PPV_ARGS(_commandList.GetAddressOf())));
	_commandList->Close();
}

void Device::CreateSyncObject()
{
	HRESULT hr = _device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(_fence.GetAddressOf()));
	_fenceValue = 1;
	_fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (FAILED(hr) || NULL == _fenceEvent) __debugbreak();
}

void Device::CreateRenderTarget()
{
	_mode.Format = _backBufferFormat;
	for (size_t i = 0; i < SWAPCHAIN_BUFFER_COUNT; i++)
	{
		_swapChainBuffer[i].Reset();
	}
	FAILED_CHECK_BREAK(
		_swapChain->ResizeBuffers(SWAPCHAIN_BUFFER_COUNT, _mode.Width, _mode.Height, _mode.Format
			, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH)
	);
	_renderTargetIndex = _swapChain->GetCurrentBackBufferIndex();

	for (UINT i = 0; i < SWAPCHAIN_BUFFER_COUNT; i++)
	{
		FAILED_CHECK_BREAK(_swapChain->GetBuffer(i, IID_PPV_ARGS(_swapChainBuffer[i].GetAddressOf())));
		_device->CreateRenderTargetView(_swapChainBuffer[i].Get(), nullptr, _renderTargetHandles[i]);
	}
}

void Device::CreateDepthStencil()
{
	D3D12_HEAP_PROPERTIES hp = {};
	hp.Type = D3D12_HEAP_TYPE_DEFAULT;
	hp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	hp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	hp.CreationNodeMask = 1;
	hp.VisibleNodeMask = 1;
	D3D12_RESOURCE_DESC rd = {};
	rd.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	rd.Alignment = 0;
	rd.Width = _mode.Width;
	rd.Height = _mode.Height;
	rd.DepthOrArraySize = 1;
	rd.MipLevels = 1;
	rd.Format = DXGI_FORMAT_R24G8_TYPELESS;
	rd.SampleDesc.Count = _4xMsaaState ? 4 : 1;
	rd.SampleDesc.Quality =
		_4xMsaaState ? (_4xMsaaQuality - 1) : 0;
	rd.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	rd.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	D3D12_CLEAR_VALUE cv = {};
	cv.Format = _depthStencilFormat;
	cv.DepthStencil.Depth = 1.f;
	cv.DepthStencil.Stencil = 0;
	FAILED_CHECK_BREAK(
		_device->CreateCommittedResource(
			&hp,
			D3D12_HEAP_FLAG_NONE,
			&rd,
			D3D12_RESOURCE_STATE_COMMON,	//D3D12_RESOURCE_STATE_COMMON 전체 파이프라인에서 '다목적' 으로 사용될 경우.
			&cv,
			IID_PPV_ARGS(_depthStencilBuffer.GetAddressOf()))
	);
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = _depthStencilFormat;
	dsvDesc.Texture2D.MipSlice = 0;
	_device->CreateDepthStencilView(_depthStencilBuffer.Get(), &dsvDesc, _depthStencilHandle);
}

HRESULT Device::CreateBuffer(UINT size, ComPtr<ID3D12Resource>& buffer)
{
	D3D12_HEAP_PROPERTIES hp = {};
	hp.Type = D3D12_HEAP_TYPE_UPLOAD;						//힙 타입 : "업로드"
	hp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	hp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	hp.CreationNodeMask = 0;
	hp.VisibleNodeMask = 0;

	//버퍼에 저장될 자원 정보 설정
	D3D12_RESOURCE_DESC rd = {};
	rd.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;		//자원 형식 : "버퍼"
	rd.Alignment = 0;									//기본 정렬 64KB (D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT)
	rd.Width = size;									//저장할 자원(데이터)의 크기.
	rd.Height = 1;
	rd.DepthOrArraySize = 1;
	rd.MipLevels = 1;
	rd.Format = DXGI_FORMAT_UNKNOWN;
	rd.SampleDesc.Count = 1;
	rd.SampleDesc.Quality = 0;
	rd.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	rd.Flags = D3D12_RESOURCE_FLAG_NONE;

	//버퍼 생성.
	ID3D12Resource* pBuff = nullptr;

	FAILED_CHECK_BREAK(
		_device->CreateCommittedResource(&hp,
			D3D12_HEAP_FLAG_NONE,
			&rd,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(buffer.GetAddressOf()))
	);
	return 0;
}

void Device::GPUSync()
{
	const UINT64 fence = _fenceValue;

	//Fence 값 갱신
	_commandQueue->Signal(_fence.Get(), fence);

	_fenceValue++;

	//GPU 의 현재 Fence 값 확인.
	if (_fence->GetCompletedValue() < fence)
	{

		//이벤트 설정 : GPU 의 펜스 값이 fence 와 동일해지면 이벤트가 발생됨.
		_fence->SetEventOnCompletion(fence, _fenceEvent);

		//대기...
		::WaitForSingleObject(_fenceEvent, INFINITE);
	}
}

void Device::SetCurrentPipelineState(ComPtr<ID3D12PipelineState> pipelineState)
{
	_currentPipelineState = pipelineState;
}

void Device::RegisterCommand(ID3D12CommandList* commandList)
{
	_commandLists.push_back(commandList);
}

void Device::ExecuteCommand()
{
	_commandQueue->ExecuteCommandLists(static_cast<UINT>(_commandLists.size()), _commandLists.data());
	_commandLists.clear();
}

void Device::ResetCommands()
{
	_commandAllocator->Reset();
	_commandList->Reset(_commandAllocator.Get(), _currentPipelineState.Get());
}

HRESULT Device::UpdateBuffer(ComPtr<ID3D12Resource>& buffer, void* data, UINT size)
{
	HRESULT hr = S_OK;
	if (nullptr == data) return hr;

	UINT8* temp = nullptr;
	hr = buffer->Map(0, nullptr, (void**)&temp);

	if (FAILED(hr) || nullptr == temp)
	{
		__debugbreak();
		return hr;
	}

	memcpy(temp, data, size);
	buffer->Unmap(0, nullptr);

	return hr;
}

HRESULT Device::ClearBackBuffer(XMVECTOR color)
{
	return 0;
}

HRESULT Device::ClearBackBuffer(UINT flag, XMVECTOR color, float depth, UINT stencil)
{
	D3D12_RECT rc = { 0, 0, (LONG)_mode.Width, (LONG)_mode.Height };

	//커맨드 리셋.
	/*_commandAllocator->Reset();
	_commandList->Reset(_commandAllocator.Get(), _currentPipelineState.Get());*/

	//장치 상태 재설정.
	_commandList->RSSetViewports(1, &_mainViewport);
	_commandList->RSSetScissorRects(1, &rc);

	//렌더타겟 상태 전환 
	//<리소스 베리어> 각 리소스의 상태관리 인터페이스. 리소스의 운용 충돌(Resource Hazard) 방지용.
	auto br = CD3DX12_RESOURCE_BARRIER::Transition(_swapChainBuffer[_renderTargetIndex].Get(), 
												   D3D12_RESOURCE_STATE_PRESENT, 
												   D3D12_RESOURCE_STATE_RENDER_TARGET);
	_commandList->ResourceBarrier(1, &br);

	_commandList->OMSetRenderTargets(1, &_renderTargetHandles[_renderTargetIndex], FALSE, &_depthStencilHandle);
	_commandList->ClearRenderTargetView(_renderTargetHandles[_renderTargetIndex], (float*)&color, 0, nullptr);
	_commandList->ClearDepthStencilView(_depthStencilHandle, (D3D12_CLEAR_FLAGS)flag, depth, stencil, 0, nullptr);

	return 0;
}

HRESULT Device::Flip()
{
	auto br = CD3DX12_RESOURCE_BARRIER::Transition(_swapChainBuffer[_renderTargetIndex].Get(), 
												   D3D12_RESOURCE_STATE_RENDER_TARGET, 
												   D3D12_RESOURCE_STATE_PRESENT);
	_commandList->ResourceBarrier(1, &br);
	_commandList->Close();

	RegisterCommand(_commandList.Get());
	ExecuteCommand();

	_swapChain->Present(0, 0);

	GPUSync();

	//새 프레임 준비.
	_renderTargetIndex = _swapChain->GetCurrentBackBufferIndex();

	return 0;
}

HRESULT Device::CreateVertexBuffer(void* data, UINT size, UINT stride, ComPtr<ID3D12Resource>& buffer, D3D12_VERTEX_BUFFER_VIEW& view)
{
	HRESULT hr = S_OK;

	//버퍼 생성
	hr = CreateBuffer(size, buffer);
	FAILED_CHECK_BREAK(hr);

	//버퍼 갱신.
	if (data) UpdateBuffer(buffer, data, size);

	//리소스-뷰 생성.		
	view.BufferLocation = buffer->GetGPUVirtualAddress();
	view.SizeInBytes = size;
	view.StrideInBytes = stride;

	return 0;
}

HRESULT Device::CreateIndexBuffer(void* data, UINT size, DXGI_FORMAT format, ComPtr<ID3D12Resource>& buffer, D3D12_INDEX_BUFFER_VIEW& view)
{
	HRESULT hr = S_OK;

	//버퍼 생성
	hr = CreateBuffer(size, buffer);
	FAILED_CHECK_BREAK(hr);

	//버퍼 갱신.
	if (data) UpdateBuffer(buffer, data, size);

	//리소스-뷰 생성.		
	view.BufferLocation = buffer->GetGPUVirtualAddress();
	view.SizeInBytes = size;
	view.Format = format;

	return 0;
}

HRESULT Device::CreateConstantBuffer(void* data, UINT size, ComPtr<ID3D12Resource>& buffer)
{
	HRESULT hr = S_OK;

	//버퍼 생성
	hr = CreateBuffer(size, buffer);
	FAILED_CHECK_BREAK(hr);

	//버퍼 갱신
	if (data) UpdateBuffer(buffer, data, size);

	//리소스-뷰 생성
	//...

	return hr;
}

HRESULT Device::CreateDefaultBuffer(UINT size, ComPtr<ID3D12Resource>& buffer)
{

	D3D12_HEAP_PROPERTIES hp = {};
	hp.Type = D3D12_HEAP_TYPE_DEFAULT;						
	hp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	hp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	hp.CreationNodeMask = 0;
	hp.VisibleNodeMask = 0;

	//버퍼에 저장될 자원 정보 설정
	D3D12_RESOURCE_DESC rd = {};
	rd.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;		//자원 형식 : "버퍼"
	rd.Alignment = 0;									//기본 정렬 64KB (D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT)
	rd.Width = size;									//저장할 자원(데이터)의 크기.
	rd.Height = 1;
	rd.DepthOrArraySize = 1;
	rd.MipLevels = 1;
	rd.Format = DXGI_FORMAT_UNKNOWN;
	rd.SampleDesc.Count = 1;
	rd.SampleDesc.Quality = 0;
	rd.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	rd.Flags = D3D12_RESOURCE_FLAG_NONE;

	//버퍼 생성.
	ID3D12Resource* pBuff = nullptr;

	FAILED_CHECK_BREAK(
		_device->CreateCommittedResource(&hp,
			D3D12_HEAP_FLAG_NONE,
			&rd,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(buffer.GetAddressOf()))
	);

	return 0;
}

