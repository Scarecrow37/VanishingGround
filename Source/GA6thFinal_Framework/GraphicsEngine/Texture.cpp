#include "pch.h"
#include "Texture.h"

#include <directxtk12/WICTextureLoader.h>
#include <directxtk12/DDSTextureLoader.h>
#include <DirectXTex.h>

Texture::Texture()
{
    Global::viewManager->AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _textureHandle, &_ID);
}

Texture::~Texture()
{
    Global::viewManager->ReturnShaderResourceDescriptorHeap(_ID);
}

bool Texture::IsValid() const
{
    return _handle.GPU.ptr != Global::dummyTextureHandle.ptr;
}

void Texture::SetResource(ID3D12Resource* resource)
{
    _resource = resource;
}

void Texture::CreateShaderResourceView()
{
    ID3D12Device* device = Global::device->GetDevice();    

    D3D12_SHADER_RESOURCE_VIEW_DESC srvd{};

    auto desc                    = _resource->GetDesc();
    srvd.Format                  = desc.Format;
    srvd.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvd.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    // srvd.Texture2D = { 0, -1, 0, 0 };											//기본지정.
    srvd.Texture2D.MipLevels = desc.MipLevels; // 밉멥레벨 수동지정.(상동)

    device->CreateShaderResourceView(_resource.Get(), &srvd, _textureHandle.CPU);

    _size.cx = (LONG)desc.Width;
    _size.cy = (LONG)desc.Height;

    _handle = _textureHandle;
}

void Texture::LoadResource(const std::filesystem::path& filePath, const std::function<void()>& callback)
{
    _handle.GPU = Global::dummyTextureHandle;

    Global::threadPool->AddTask(ThreadPool::ThreadType::PARALLEL, [this, filePath, callback](ID3D12GraphicsCommandList*)
        {
            HRESULT       hr     = S_OK;
            ID3D12Device* device = Global::device->GetDevice();

            // 0. 텍스처용 메모리 Upload Heap 준비 : DXTK 사용
            ResourceUploadBatch resUpload(device);
            resUpload.Begin();
            if (filePath.extension() == L".dds")
            {
                hr = CreateDDSTextureFromFileEx(device, resUpload, filePath.c_str(), 0, D3D12_RESOURCE_FLAG_NONE, DDS_LOADER_IGNORE_SRGB | DDS_LOADER_MIP_AUTOGEN, &_resource);
            }
            else if (filePath.extension() == L".tga")
            {
                __debugbreak();
            }
            else
            {
                hr = CreateWICTextureFromFileEx(device, resUpload, filePath.c_str(), 0, D3D12_RESOURCE_FLAG_NONE, WIC_LOADER_IGNORE_SRGB | WIC_LOADER_MIP_AUTOGEN, &_resource);
            }
            FAILED_CHECK_MESSAGE(hr, L"Texture::LoadResource Failed");

            resUpload.End(Global::commandController->GetCommandQueue(CommandQueueType::GRAPHICS_QUEUE));

            CreateShaderResourceView();
            
            Global::resourceManager->EnqueueCallback(callback);
        });
}