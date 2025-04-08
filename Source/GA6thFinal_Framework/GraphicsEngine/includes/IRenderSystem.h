#pragma once
#include "IBash.h"

namespace GE
{
	struct RENDERER_DESC
	{
		HWND			hWnd;
		const wchar_t*	shaderRootPath;
		float			width;
		float			height;
		enum class Type { DX11, DX12, DXR };
		Type			type;
		unsigned int	maxLayer;
		bool			isFullScreen;
	};

	struct MESH_RENDERER_DESC
	{
		const wchar_t*	filePath;
		enum class Type { Static, Skeletal, SkyBox };
		Type			type;
	};

	class IMeshRenderer;
	class IAnimator;
	class Matrix4x4;
	class ITextSystem;
	class IPostProcessSystem;
	class ISpriteSystem;
	class IUnlitSystem;
	class IRenderSystem : public IBase
	{
	protected:
		explicit IRenderSystem() = default;
		virtual ~IRenderSystem() = default;
		IRenderSystem(const IRenderSystem&) = delete;
		IRenderSystem(IRenderSystem&&) = delete;
		IRenderSystem& operator=(const IRenderSystem&) = delete;
		IRenderSystem& operator=(IRenderSystem&&) = delete;

	public:
		virtual void Initialize(const RENDERER_DESC* pDesc) = 0;
		virtual void Render() = 0;

		virtual void GetTextSystem(ITextSystem** ppTextSystem) = 0;
		virtual void GetPostProcessSystem(IPostProcessSystem** ppPostProcessSystem) = 0;
		virtual void GetSpriteSystem(ISpriteSystem** ppSpriteSystem) = 0;
		virtual void GetUnlitSystem(IUnlitSystem** ppUnlitSystem) = 0;
		virtual void CreateMeshRenderer(IMeshRenderer** ppComponent, const MESH_RENDERER_DESC* pDesc) = 0;		
		virtual void RegisterRenderQueue(IMeshRenderer* pComponent, Matrix4x4* pMatrix) = 0;
		virtual void UnRegisterRenderQueue(IMeshRenderer* pComponent) = 0;
	};
}