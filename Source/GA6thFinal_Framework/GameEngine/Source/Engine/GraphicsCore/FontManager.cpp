#include "pch.h"
#include "FontManager.h"
#include "Font.h"

FontManager::FontManager()
	: _offset(0)
	, _numFonts(0)
	, _maxFonts(0)
{
}

FontManager::~FontManager()
{
}

HRESULT FontManager::Initialize(const UINT numFonts)
{
	HRESULT hr = S_OK;
	ComPtr<ID3D12Device> device = UmDevice.GetDevice();

	D3D12_DESCRIPTOR_HEAP_DESC desc
	{
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		.NumDescriptors = numFonts,
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
		.NodeMask = 0,
	};

	hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(_fontHeap.GetAddressOf()));
	FAILED_CHECK_BREAK(hr);

	try
	{
		// 임시로 백버퍼에 폰트를 출력
		// 추후에는 UI 전용 타겟에 z 정렬 후 출력할 예정		
		RenderTargetState rtState(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_D24_UNORM_S8_UINT);
		//SpriteBatchPipelineStateDescription psd(rtState);
		ResourceUploadBatch resourceUpload(device.Get());
		resourceUpload.Begin();
		
		D3D12_VIEWPORT viewport = UmDevice.GetMainViewport();
		//_spriteBatch = std::make_shared<SpriteBatch>(device.Get(), resourceUpload, psd, &viewport);

		//auto uploadFinish = resourceUpload.End(Global::device.GetCommandQueue().Get());
		//uploadFinish.wait();
	}
	catch (std::exception& e)
	{
		std::filesystem::path msg = e.what();	
		ASSERT(false, L"Font creation failed!");
	}

	_maxFonts = numFonts;
	_offset = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    return hr;
}

std::shared_ptr<Font> FontManager::LoadFont(std::wstring_view filePath)
{
	if (_fonts.end() == _fonts.find(filePath.data()))
	{
		if (_numFonts >= _maxFonts)
		{
			ASSERT(false, L"Font creation failed!\n Maximum number exceeded");
			return nullptr;
		}

		std::shared_ptr<Font> font = UmResourceManager.LoadResource<Font>(filePath.data());

		D3D12_CPU_DESCRIPTOR_HANDLE cpu = _fontHeap->GetCPUDescriptorHandleForHeapStart();
		D3D12_GPU_DESCRIPTOR_HANDLE gpu = _fontHeap->GetGPUDescriptorHandleForHeapStart();
		cpu.ptr += _offset * _numFonts;
		gpu.ptr += _offset * _numFonts;

		font->Initialize(cpu, gpu);

		_fonts[filePath.data()] = font;
		_numFonts++;
	}

    return _fonts[filePath.data()];
}

void FontManager::Render()
{
	for (auto& [tag, font] : _fonts)
	{		
		font->Draw(_spriteBatch);
	}
}
