#pragma once
#include "Resource.h"

class Animation : public Resource
{
	friend class Animator;

	struct BoneTransformTrack
	{
		std::vector<std::pair<float, Vector3>> positions;
		std::vector<std::pair<float, Vector4>> rotations;
		std::vector<std::pair<float, Vector3>> scales;
	};

	struct Channel
	{
		std::unordered_map<std::string, BoneTransformTrack> boneTransforms;
		float lastTime = 0.f;
	};

public:
	explicit Animation() = default;
	virtual ~Animation() = default;

public:
	void LoadAnimation(const aiScene* paiScene);
	// Resource을(를) 통해 상속됨
	HRESULT LoadResource(const std::filesystem::path& filePath) override;

private:
	// Resource을(를) 통해 상속됨
	void Free() override;

private:
	std::unordered_map<std::string, Channel> _animations;	
};