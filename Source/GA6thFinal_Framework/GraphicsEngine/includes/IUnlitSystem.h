#pragma once

namespace GE
{
	struct UNLIT_RENDERER_DESC
	{
		enum class Type { Line };
		Type type;
	};

	class IUnlitRenderer;
	class Matrix4x4;
	class IUnlitSystem
	{
	protected:
		explicit IUnlitSystem() = default;
		virtual ~IUnlitSystem() = default;
		IUnlitSystem(const IUnlitSystem&) = delete;
		IUnlitSystem(IUnlitSystem&&) = delete;
		IUnlitSystem& operator=(const IUnlitSystem&) = delete;
		IUnlitSystem& operator=(IUnlitSystem&&) = delete;

	public:
		virtual void CreateUnlitRenderer(IUnlitRenderer** ppComponent, const UNLIT_RENDERER_DESC* desc) = 0;
		virtual void RegisterRenderQueue(IUnlitRenderer* pComponent, GE::Matrix4x4* pMatrix) = 0;
		virtual void UnRegisterRenderQueue(IUnlitRenderer* pComponent) = 0;
	};
}