#include "pch.h"
#include "Texture.h"

#include <directxtk12/WICTextureLoader.h>
#include <directxtk12/DDSTextureLoader.h>
#include <DirectXTex.h>

void Texture::SetResource(ID3D12Resource* resource)
{
    _resource = resource;
}

void Texture::CreateShaderResourceView()
{
    ID3D12Device* device = UmDevice.GetDevice().Get();
    HRESULT hr = UmViewManager.AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _handle);
    FAILED_CHECK_BREAK(hr);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvd = {};

    srvd.Format                  = _resource->GetDesc().Format;
    srvd.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvd.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    // srvd.Texture2D = { 0, -1, 0, 0 };											//기본지정.
    srvd.Texture2D.MipLevels = _resource->GetDesc().MipLevels; // 밉멥레벨 수동지정.(상동)

    device->CreateShaderResourceView(_resource.Get(), &srvd, _handle);
}

HRESULT Texture::LoadResource(const std::filesystem::path& filePath)
{
	HRESULT hr = S_OK;
	ID3D12Device* device = UmDevice.GetDevice().Get();

	//0. 텍스처용 메모리 Upload Heap 준비 : DXTK 사용
	ResourceUploadBatch resUpload(device);
	resUpload.Begin();

	if (filePath.extension() == L".dds")
	{
		hr = CreateDDSTextureFromFile(device, resUpload, filePath.c_str(), _resource.GetAddressOf(), true);
	}
	else if (filePath.extension() == L".tga")
	{
		auto newPath = filePath;
		newPath.replace_extension(L".dds");
		hr = CreateDDSTextureFromFile(device, resUpload, newPath.c_str(), _resource.GetAddressOf(), true);
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
		hr = CreateWICTextureFromFile(device, resUpload, filePath.c_str(), _resource.GetAddressOf(), true);
	}	
	FAILED_CHECK_BREAK(hr);

    resUpload.End(UmDevice.GetCommandQueue().Get());

	CreateShaderResourceView();

	return hr;
}
