#pragma once
#include "Base.h"
#include "includes/IUnlitSystem.h"

class UnlitRenderer;
class UnlitSystem : public Base, public GE::IUnlitSystem
{
public:
	explicit UnlitSystem() = default;
	virtual ~UnlitSystem() = default;

public:
	// IUnlitSystem을(를) 통해 상속됨
	void CreateUnlitRenderer(GE::IUnlitRenderer** ppComponent, const GE::UNLIT_RENDERER_DESC* desc) override;
	void RegisterRenderQueue(GE::IUnlitRenderer* pComponent, GE::Matrix4x4* pMatrix) override;
	void UnRegisterRenderQueue(GE::IUnlitRenderer* pComponent) override;

public:
	void Initialize();
	void Render();

private:
	// Base을(를) 통해 상속됨
	void Free() override;

private:
	std::vector<std::pair<GE::IUnlitRenderer*, Matrix*>> _components;
	ID3D11DeviceContext* _pDeviceContext{ nullptr };
	ID3D11RasterizerState* _pRSState{ nullptr };
};