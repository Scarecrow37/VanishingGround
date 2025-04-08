#pragma once
#include "Base.h"
#include "includes/ISpriteSystem.h"

class SpriteRenderer;
class PixelShader;
class SpriteSystem : public Base, public GE::ISpriteSystem
{	
public:
	explicit SpriteSystem() = default;
	virtual ~SpriteSystem() = default;

public:
	// ISpriteSystem을(를) 통해 상속됨
	void CreateTextRenderer(GE::ISpriteRenderer** ppComponent, const wchar_t* filePath) override;
	void RegisterRenderQueue(GE::ISpriteRenderer* pComponent, GE::Matrix4x4* pMatrix) override;
	void UnRegisterRenderQueue(GE::ISpriteRenderer* pComponent) override;

public:
	void Initialize();
	void Render();

private:
	// Base을(를) 통해 상속됨
	void Free() override;

private:
	std::vector<std::pair<SpriteRenderer*, Matrix*>>	_components;
	std::shared_ptr<PixelShader>						_psSprite;
	ID3D11DeviceContext*								_pDeviceContext{ nullptr };
};