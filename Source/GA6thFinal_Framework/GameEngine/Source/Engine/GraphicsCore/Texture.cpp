#include "pch.h"
#include "Texture.h"

#include <directxtk12/WICTextureLoader.h>
#include <directxtk12/DDSTextureLoader.h>
#include <DirectXTex.h>

HRESULT Texture::LoadResource(const std::filesystem::path& filePath)
{
	HRESULT hr = S_OK;
	ComPtr<ID3D12Device> device = UmDevice.GetDevice();

	//0. 텍스처용 메모리 Upload Heap 준비 : DXTK 사용
	ResourceUploadBatch resUpload(device.Get());
	resUpload.Begin();

	if (filePath.extension() == L".dds")
	{
		hr = CreateDDSTextureFromFile(device.Get(), resUpload, filePath.c_str(), _resource.GetAddressOf(), true);
	}
	else if (filePath.extension() == L".tga")
	{
		auto newPath = filePath;
		newPath.replace_extension(L".dds");
		hr = CreateDDSTextureFromFile(device.Get(), resUpload, newPath.c_str(), _resource.GetAddressOf(), true);
	}
	/*else if (filePath.extension() == L".tga")
	{
		ScratchImage image;
		TexMetadata metadata;
		hr = LoadFromTGAFile(filePath.c_str(), &metadata, image);
		FAILED_CHECK_BREAK(hr);

		ComPtr<ID3D12Resource> texture;
		ComPtr<ID3D12Resource> textureUpload;
		hr = CreateTexture(device.Get(), metadata, textureUpload.GetAddressOf());
		FAILED_CHECK_BREAK(hr);

		D3D12_SUBRESOURCE_DATA subresourceData = {};
		UpdateSubresources(Global::device.GetCommandList().Get(), texture.Get(), textureUpload.Get(), 0, 0, 1, &subresourceData);
	}*/
	else
	{
		hr = CreateWICTextureFromFile(device.Get(), resUpload, filePath.c_str(), _resource.GetAddressOf(), true);
	}	
	FAILED_CHECK_BREAK(hr);

	hr = UmViewManager.AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _handle);
	FAILED_CHECK_BREAK(hr);

	//2.2 셰이더 리소스뷰 디스크립터 구성 생성.
	D3D12_SHADER_RESOURCE_VIEW_DESC srvd = {};

	//srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvd.Format = _resource->GetDesc().Format;
	srvd.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvd.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//srvd.Texture2D = { 0, -1, 0, 0 };											//기본지정.
	srvd.Texture2D.MipLevels = _resource->GetDesc().MipLevels;					//밉멥레벨 수동지정.(상동) 

	//2.3 셰이더 리소스뷰 생성. 
	//    SRV Desc 힙 -> (텍스처별) Desc 주소 -> (그곳에) SRV 생성
	//    텍스처가 여러개 사용된다면, 힙 내부의 Desc 별 접근 필요.(주소 offset 계산)	
	device->CreateShaderResourceView(_resource.Get(), &srvd, _handle);

	if (nullptr == _resource)
	{
		__debugbreak();
		return hr;
	}

	//텍스처 로딩, 메모리 작업 완료.(Upload Heap)
	resUpload.End(UmDevice.GetCommandQueue().Get());

	return hr;
}
