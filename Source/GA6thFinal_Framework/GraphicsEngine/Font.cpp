#include "pch.h"
#include "Font.h"

Font::Font()
{
}

Font::~Font()
{
}

void Font::LoadResource(const std::filesystem::path& filePath, const std::function<void()>& callback)
{
    ID3D12Device*       device = Global::device->GetDevice();
    ResourceUploadBatch resourceUpload(device);

    Global::viewManager->AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _handle);

    resourceUpload.Begin();

    _font = std::make_unique<DirectX::DX12::SpriteFont>(device, resourceUpload, filePath.c_str(), _handle.CPU, _handle.GPU);
    auto uploadFinish = resourceUpload.End(Global::commandController->GetCommandQueue(CommandQueueType::GRAPHICS_QUEUE));

    uploadFinish.wait();

    _font->SetDefaultCharacter(L'_');
}