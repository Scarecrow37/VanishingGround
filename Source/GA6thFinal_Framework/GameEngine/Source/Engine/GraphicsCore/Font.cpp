#include "pch.h"
#include "Font.h"

Font::Font()
	: _maxTextLength(0)
	, _color(1.0f, 1.0f, 1.0f, 1.0f)
{
}

Font::~Font()
{
}

void Font::SetMaxTextLength(const UINT size)
{
	_text.resize(size);
}

void Font::SetText(std::wstring_view text)
{
	if (text.size() > _maxTextLength)
	{
		GRAPHICS_ASSERT(false, L"Font text size is too long.");
		return;
	}

	lstrcpy(_text.data(), text.data());
}

void Font::LoadResource(const std::filesystem::path& filePath)
{
	_filePath = filePath;
}

void Font::Initialize(D3D12_CPU_DESCRIPTOR_HANDLE cpu, D3D12_GPU_DESCRIPTOR_HANDLE gpu)
{
	ID3D12Device* device = UmDevice.GetDevice();
	ResourceUploadBatch resourceUpload(device);
	resourceUpload.Begin();

	_font = std::make_unique<DirectX::DX12::SpriteFont>(device, resourceUpload, _filePath.c_str(), cpu, gpu);
	auto uploadFinish = resourceUpload.End(UmDevice.GetCommandQueue());

	uploadFinish.wait();

	_font->SetDefaultCharacter(L'_');
	_maxTextLength = 256;
	_text.resize(_maxTextLength);
}

void Font::Draw(std::shared_ptr<SpriteBatch>& spriteBatch)
{
	//_font->DrawString(spriteBatch.get(), _text.c_str(), _position, _color, _rotation, _origin, _scale);
	_font->DrawString(spriteBatch.get(), _text.c_str(), _position, _color, 0.0f, _origin, _scale);
}
