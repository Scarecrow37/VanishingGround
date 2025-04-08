#pragma once
#include "Component.h"
#include "includes/ISpriteRenderer.h"

class Texture;
class SpriteRenderer : public Component, public GE::ISpriteRenderer
{
	struct CameraData
	{
		Matrix view;
		Matrix projection;
		Vector3 cameraPosition;
	};
public:
	explicit SpriteRenderer() = default;
	virtual ~SpriteRenderer() = default;

public:
	GE::SpriteDescription::Type GetType() const { return _description.type; }

public:
	void Initialize(const wchar_t* filePath);
	void Render(ID3D11DeviceContext* pDeviceContext);

public:
	// ISpriteRenderer을(를) 통해 상속됨
	void Release() override;
	void GetDesc(GE::SpriteDescription* out) override;
	void SetDesc(const GE::SpriteDescription* in) override;
	void GetImageSize(float* outWidth, float* outHeight) override;

private:
	GE::SpriteDescription _description;
	std::shared_ptr<Texture> _texture;
};