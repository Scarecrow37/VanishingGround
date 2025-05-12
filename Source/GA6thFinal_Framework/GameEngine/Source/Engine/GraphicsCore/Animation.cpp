#include "pch.h"
#include "Animation.h"

void Animation::LoadAnimation(const aiScene* scene)
{
	auto AssimpVec3ToSimpleMathVec3 = [](const aiVector3D& in) { return XMVectorSet(in.x, in.y, in.z, 0.f); };
	auto AssimpQuatToSimpleMathQuat = [](const aiQuaternion& in) { return XMVectorSet(in.x, in.y, in.z, in.w); };

	for (unsigned int i = 0; i < scene->mNumAnimations; i++)
	{
		Channel animation;
		aiAnimation* anim = scene->mAnimations[i];

		float offset = 1.f / (float)anim->mTicksPerSecond;
		
		for (unsigned int j = 0; j < anim->mNumChannels; j++)
		{
			aiNodeAnim* channel = anim->mChannels[j];
			BoneTransformTrack track;

			for (unsigned int k = 0; k < channel->mNumPositionKeys; k++)
			{				
				track.positions.emplace_back(
					(float)channel->mPositionKeys[k].mTime * offset,
					AssimpVec3ToSimpleMathVec3(channel->mPositionKeys[k].mValue));
			}

			for (unsigned int k = 0; k < channel->mNumRotationKeys; k++)
			{
				track.rotations.emplace_back(
					(float)channel->mRotationKeys[k].mTime * offset,
					AssimpQuatToSimpleMathQuat(channel->mRotationKeys[k].mValue));
			}

			for (unsigned int k = 0; k < channel->mNumScalingKeys; k++)
			{
				track.scales.emplace_back(
					(float)channel->mScalingKeys[k].mTime * offset,
					AssimpVec3ToSimpleMathVec3(channel->mScalingKeys[k].mValue));
			}

			animation.boneTransforms[channel->mNodeName.C_Str()] = track;
			animation.lastTime = std::max(track.scales.back().first, track.rotations.back().first);
			animation.lastTime = std::max(animation.lastTime, track.positions.back().first);
		}
        
		_animations[anim->mName.C_Str()] = animation;
	}
}

HRESULT Animation::LoadResource(const std::filesystem::path& filePath)
{
	return S_OK;
}