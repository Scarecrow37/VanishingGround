#include "pch.h"
#include "Font.h"

Font::Font()
{
}

Font::~Font()
{
}

void Font::LoadResource(const std::filesystem::path& filePath)
{
    ID3D12Device*       device = UmDevice.GetDevice();
    ResourceUploadBatch resourceUpload(device);

    UmViewManager.AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _handle);

    resourceUpload.Begin();

    _font = std::make_unique<DirectX::DX12::SpriteFont>(device, resourceUpload, filePath.c_str(), _handle.CPU, _handle.GPU);
    auto uploadFinish = resourceUpload.End(UmDevice.GetCommandQueue());

    uploadFinish.wait();

    _font->SetDefaultCharacter(L'_');
}