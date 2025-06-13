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
    ID3D12Device* device = UmDevice.GetDevice();
    UmViewManager.AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _handle);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvd{};

    srvd.Format                  = _resource->GetDesc().Format;
    srvd.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvd.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    // srvd.Texture2D = { 0, -1, 0, 0 };											//기본지정.
    srvd.Texture2D.MipLevels = _resource->GetDesc().MipLevels; // 밉멥레벨 수동지정.(상동)

    device->CreateShaderResourceView(_resource.Get(), &srvd, _handle);
}

void Texture::LoadResource(const std::filesystem::path& filePath)
{
	HRESULT hr = S_OK;
	ID3D12Device* device = UmDevice.GetDevice();

	//0. 텍스처용 메모리 Upload Heap 준비 : DXTK 사용
	ResourceUploadBatch resUpload(device);
	resUpload.Begin();

	if (filePath.extension() == L".dds")
	{
		hr = CreateDDSTextureFromFile(device, resUpload, filePath.c_str(), &_resource, true);
	}
	else if (filePath.extension() == L".tga")
	{
        __debugbreak();
	}
	else
	{
		hr = CreateWICTextureFromFile(device, resUpload, filePath.c_str(), &_resource, true);
	}	
	FAILED_CHECK_MESSAGE(hr, L"Texture::LoadResource Failed");

    resUpload.End(UmDevice.GetCommandQueue());

	CreateShaderResourceView();
}
