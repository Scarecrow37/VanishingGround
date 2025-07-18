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
				track.Positions.emplace_back(
					(float)channel->mPositionKeys[k].mTime * offset,
					AssimpVec3ToSimpleMathVec3(channel->mPositionKeys[k].mValue));
			}

			for (unsigned int k = 0; k < channel->mNumRotationKeys; k++)
			{
				track.Rotations.emplace_back(
					(float)channel->mRotationKeys[k].mTime * offset,
					AssimpQuatToSimpleMathQuat(channel->mRotationKeys[k].mValue));
			}

			for (unsigned int k = 0; k < channel->mNumScalingKeys; k++)
			{
				track.Scales.emplace_back(
					(float)channel->mScalingKeys[k].mTime * offset,
					AssimpVec3ToSimpleMathVec3(channel->mScalingKeys[k].mValue));
			}

			animation.BoneTransforms[channel->mNodeName.C_Str()] = track;
            animation.LastTime = (std::max)({track.Scales.back().first, 
                                           track.Rotations.back().first,
                                           track.Positions.back().first, 
                                           animation.LastTime});
		}
        
		_animations[anim->mName.C_Str()] = animation;
	}

    for (auto& [name, channel] : _animations)
    {
        _animationNames.push_back(name.data());
    }
}

void Animation::LoadResource(const std::filesystem::path& filePath)
{
}