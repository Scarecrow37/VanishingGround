#pragma once
#include "IBash.h"

namespace GE
{	
	class ICameraSystem;
	class IRenderSystem;
	class IAnimationSystem;
	class ILightSystem;	
	class IGraphicsSystem : public IBase
	{
	protected:
		explicit IGraphicsSystem() = default;
		virtual ~IGraphicsSystem() = default;
		IGraphicsSystem(const IGraphicsSystem&) = delete;
		IGraphicsSystem(IGraphicsSystem&&) = delete;
		IGraphicsSystem& operator=(const IGraphicsSystem&) = delete;
		IGraphicsSystem& operator=(IGraphicsSystem&&) = delete;

	public:
		virtual void CreateAnimationSystem(IAnimationSystem** ppAnimationSystem) = 0;
		virtual void CreateCameraSystem(ICameraSystem** ppCameraSystem) = 0;
		virtual void CreateLightSystem(ILightSystem** ppLightSystem) = 0;
		virtual void CreateRenderSystem(IRenderSystem** ppRenderSystem) = 0;
        virtual void GetDevice(ID3D11Device** ppDevice) = 0;
	};
}