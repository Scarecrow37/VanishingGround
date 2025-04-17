#pragma once
#include "Resource.h"

class Font : public Resource
{
	friend class FontManager;
public:
	Font();
	virtual ~Font();

public:
	void SetColor(const Vector4& color) { _color = color; }
	void SetPosition(const Vector3& position) { _position = position; }
	void SetScale(const Vector2& scale) { _scale = scale; }
	void SetRotation(const Vector2& rotation) { _rotation = rotation; }
	void SetOrigin(const Vector2& origin) { _origin = origin; }
	void SetMaxTextLength(const UINT size);
	void SetText(std::wstring_view text);

public:
	// Resource을(를) 통해 상속됨
	HRESULT LoadResource(const std::filesystem::path& filePath) override;
	void Initialize(D3D12_CPU_DESCRIPTOR_HANDLE cpu, D3D12_GPU_DESCRIPTOR_HANDLE gpu);

private:
	void Draw(std::shared_ptr<SpriteBatch>& spriteBatch);

private:
	std::wstring				_filePath;
	std::wstring				_text;
	Vector4						_color;
	Vector3						_position;
	Vector2						_scale;
	Vector2						_rotation;
	Vector2						_origin;
	std::unique_ptr<SpriteFont> _font;
	UINT 						_maxTextLength;
};