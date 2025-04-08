#pragma once
#include "MeshRenderer.h"

class Animator;
class SkeletalMeshRenderer : public MeshRenderer
{
public:
	explicit SkeletalMeshRenderer() = default;
	virtual ~SkeletalMeshRenderer() = default;

public:
	Animator* GetAnimator() { return _pAnimator; }

public:
	// MeshRenderer��(��) ���� ��ӵ�
	void Initialize(const wchar_t* filePath) override;

private:
	Animator* _pAnimator{ nullptr };
};