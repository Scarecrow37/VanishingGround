#pragma once
#include "includes/IRenderSystem.h"

class IRenderer;
class TextSystem;
class SpriteSystem;
class UnlitSystem;
class PostProcessSystem;
class RenderSystem : public GE::IRenderSystem
{
public:
	explicit RenderSystem() = default;
	virtual ~RenderSystem() = default;

public:
	// IRenderSystem을(를) 통해 상속됨
	void Initialize(const GE::RENDERER_DESC* pDesc) override;
	void Release() override;
	void Render() override;

	void GetTextSystem(GE::ITextSystem** ppTextSystem) override;
	void GetPostProcessSystem(GE::IPostProcessSystem** ppPostProcessSystem) override;
	void GetSpriteSystem(GE::ISpriteSystem** ppSpriteSystem) override;
	void GetUnlitSystem(GE::IUnlitSystem** ppUnlitSystem) override;
	void CreateMeshRenderer(GE::IMeshRenderer** ppComponent, const GE::MESH_RENDERER_DESC* pDesc) override;
	void RegisterRenderQueue(GE::IMeshRenderer* pComponent, GE::Matrix4x4* pMatrix) override;
	void UnRegisterRenderQueue(GE::IMeshRenderer* pComponent) override;

private:
	void InitializeDX11(HWND hWnd, bool isFullScreen, const unsigned int maxLayer);
	void InitializeDX12();
	void InitializeDXR();

private:
	IRenderer*			_pRenderer{ nullptr };
	TextSystem*			_pTextSystem{ nullptr };
	SpriteSystem*		_pSpriteSystem{ nullptr };
	PostProcessSystem*	_pPostProcessSystem{ nullptr };
	UnlitSystem*		_pUnlitSystem{ nullptr };
	IDXGISwapChain*		_pSwapChain{ nullptr };
};