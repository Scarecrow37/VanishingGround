#pragma once
#include "IBash.h"

namespace GE
{
	class IAnimator;
	class IMeshRenderer;
	class IAnimationSystem : public IBase
	{
	protected:
		explicit IAnimationSystem() = default;
		virtual ~IAnimationSystem() = default;
		IAnimationSystem(const IAnimationSystem&) = delete;
		IAnimationSystem(IAnimationSystem&&) = delete;
		IAnimationSystem& operator=(const IAnimationSystem&) = delete;
		IAnimationSystem& operator=(IAnimationSystem&&) = delete;

	public:
		virtual void Initialize(const unsigned int maxThread) = 0;
		virtual void Update(const float deltaTime) = 0;
		virtual void CreateAnimator(IMeshRenderer* pMeshRenderer, IAnimator** ppOutAnimator) = 0;
		virtual void RegisterAnimator(IAnimator* pAnimator) = 0;
		virtual void UnRegisterAnimator(IAnimator* pAnimator) = 0;
	};
}