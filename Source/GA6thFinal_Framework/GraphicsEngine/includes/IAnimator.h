#pragma once
#include "IBash.h"

namespace GE
{
	class Matrix4x4;
	class IAnimator : public IBase
	{
	protected:
		explicit IAnimator() = default;
		virtual ~IAnimator() = default;
		IAnimator(const IAnimator&) = delete;
		IAnimator(IAnimator&&) = delete;
		IAnimator& operator=(const IAnimator&) = delete;
		IAnimator& operator=(IAnimator&&) = delete;

	public:
		virtual void ChangeAnimation(const char* animation) = 0;
		virtual void ChangeAnimation(const char* animation, unsigned int ID) = 0;
		virtual void SyncPartialAnimation(unsigned int parentID, unsigned int childID) = 0;
		virtual bool IsLastFrame(float interval, unsigned int ID) const = 0;
		virtual void SetUpSplitBone(unsigned int maxSplit) = 0;
		virtual void SplitBone(unsigned int ID, const char* boneName) = 0;
		virtual void SetAnimationSpeed(float speed) = 0;
		virtual void SetAnimationSpeed(float speed, unsigned int ID) = 0;
		virtual void MakeParent(const char* parent, const char* child) = 0;
		virtual void GetSkeletonMatrix(const char* bone, Matrix4x4** out) = 0;
	};
}