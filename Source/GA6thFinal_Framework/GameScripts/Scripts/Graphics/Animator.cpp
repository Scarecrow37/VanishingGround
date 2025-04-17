//#include "pch.h"
//#include "Animator.h"
//#include "Engine/GraphicsCore/Skeleton.h"
//#include "Engine/GraphicsCore/Animation.h"
//
//unsigned int Animator::_globalID = 0;
//constexpr unsigned int MAX_BONE_MATRIX = 256;
//
//Animator::Animator()
//	: _ID(_globalID++)
//	, _maxSplit(1)
//{
//}
//
//void Animator::Initialize(const wchar_t* filePath, Skeleton* pSkeleton)
//{
//	_animation = Global::resourceManager->LoadResource<Animation>(filePath);
//	_pSkeleton = pSkeleton;
//	_controllers.resize(1);
//	_prevControllers.resize(1);
//	_blends.resize(1);
//
//	_animationTransforms.resize(MAX_BONE_MATRIX);
//	_currTransforms.resize(MAX_BONE_MATRIX);
//	_prevTransforms.resize(MAX_BONE_MATRIX);
//	_blendMatrixMask.resize(MAX_BONE_MATRIX);
//}
//
//void Animator::Update(const float deltaTime)
//{
//	XMMATRIX identity = XMMatrixIdentity();
//
//	for (unsigned int i = 0; i < _maxSplit; i++)
//	{
//		const Animation::Channel& animation = _animation->_animations[_controllers[i].animation];
//		_controllers[i].playTime += _controllers[i].speed * deltaTime;
//	
//		if (_controllers[i].playTime >= animation.lastTime)
//		{
//			_controllers[i].playTime = fmod(_controllers[i].playTime, animation.lastTime);
//		}
//	}
//
//	UpdateAnimationTransform(_pSkeleton->GetRootBone(), identity, _controllers, _animationTransforms);
//
//	if (_isBlending)
//	{
//		memcpy(_currTransforms.data(), _animationTransforms.data(), sizeof(Matrix) * MAX_BONE_MATRIX);
//		UpdateAnimationTransform(_pSkeleton->GetRootBone(), identity, _prevControllers, _prevTransforms);
//
//		unsigned int count = 0;
//		for (unsigned int i = 0; i < _maxSplit; i++)
//		{
//			if (_blends[i].isBlending)
//			{
//				if (1.f <= _blends[i].blendTime)
//				{
//					_blends[i].blendTime = 0.f;
//					_blends[i].isBlending = false;
//				}
//				else
//				{
//					float t = 1 - (1 - _blends[i].blendTime) * (1 - _blends[i].blendTime);
//
//					for (size_t j = 0; j < MAX_BONE_MATRIX; j++)
//					{
//						if (i != _blendMatrixMask[j])
//						{
//							//_animationTransforms[j] = _currTransforms[j];
//						}
//						else
//						{
//							_animationTransforms[j] = BlendAnimation(_prevTransforms[j], _currTransforms[j], t);
//						}
//					}
//
//					_blends[i].blendTime += deltaTime * 5.f;
//				}
//			}
//
//			if (!_blends[i].isBlending)
//				count++;
//		}
//
//		if (count == _maxSplit)
//			_isBlending = false;
//	}
//}
//
//void Animator::Release()
//{
//	delete this;
//}
//
//void Animator::ChangeAnimation(const char* animation)
//{
//	for (unsigned int i = 0; i < _maxSplit; i++)
//		ChangeAnimation(animation, i);
//}
//
//void Animator::ChangeAnimation(const char* animation, const unsigned int ID)
//{
//	auto iter = _animation->_animations.find(animation);
//	if (iter == _animation->_animations.end())
//		return;
//
//	if (!strcmp(_controllers[ID].animation, animation))
//		return;
//
//	_isBlending = true;
//	_blends[ID].blendTime = 0.f;
//	_blends[ID].isBlending = true;
//
//	_prevControllers[ID] = _controllers[ID];
//	_controllers[ID].animation = animation;
//	_controllers[ID].playTime = 0.f;
//	_controllers[ID].lastTime = iter->second.lastTime;
//}
//
//void Animator::SyncPartialAnimation(unsigned int parentID, unsigned int childID)
//{
//	if (parentID >= _maxSplit || childID >= _maxSplit)
//		return;
//
//	_isBlending = true;
//	_blends[childID].blendTime = 0.f;
//	_blends[childID].isBlending = true;
//
//	_prevControllers[childID] = _controllers[childID];
//	//_prevControllers[parentID] = _controllers[parentID];
//	_controllers[childID] = _controllers[parentID];
//}
//
//bool Animator::IsLastFrame(float interval, const unsigned int ID) const
//{
//	return _controllers[ID].playTime + interval > _controllers[ID].lastTime;
//}
//
//void Animator::SetUpSplitBone(const unsigned int maxSplit)
//{
//	_pSkeleton->SetUpSplitBone(maxSplit);
//	_maxSplit = maxSplit;
//
//	_controllers.resize(maxSplit);
//	_prevControllers.resize(maxSplit);
//	_blends.resize(maxSplit);
//}
//
//void Animator::SplitBone(const unsigned int ID, const char* boneName)
//{
//	if (_maxSplit <= ID)
//	{
//		ASSERT(false, L"Greater than the number of bones you set.");
//		return;
//	}
//
//	_pSkeleton->SplitBone(ID, boneName);
//	BoneMasking(_pSkeleton->GetBone(ID), ID);
//}
//
//void Animator::SetAnimationSpeed(float speed)
//{
//	for (unsigned int i = 0; i < _maxSplit; i++)
//	{
//		_controllers[i].speed = speed;
//	}
//}
//
//void Animator::SetAnimationSpeed(float speed, unsigned int ID)
//{
//	if (_maxSplit <= ID)
//	{
//		ASSERT(false, L"Greater than the number of bones you set.");
//		return;
//	}
//
//	_controllers[ID].speed = speed;
//}
//
//void Animator::MakeParent(const char* parent, const char* child)
//{
//	_pSkeleton->MakeParent(parent, child);
//}
//
////void Animator::GetSkeletonMatrix(const char* bone, GE::Matrix4x4** out)
////{
////	Bone& rootBone = _pSkeleton->GetRootBone();
////
////	std::queue<Bone*> bfs;
////	bfs.push(&rootBone);
////
////	while (!bfs.empty())
////	{
////		Bone* curr = bfs.front();
////		bfs.pop();
////
////		if (curr->name == bone)
////		{
////			(*out) = (GE::Matrix4x4*)&curr->anim;
////			break;
////		}
////		for (auto& child : curr->children)
////		{
////			bfs.push(&child);
////		}
////	}
////}
//
//void Animator::UpdateAnimationTransform(Bone& skeletion, 
//										const XMMATRIX& parentTransform, 
//										std::vector<Controller>& controllers, 
//										std::vector<Matrix>& transforms)
//{
//	Controller& current = controllers[_boneMask[skeletion.name]];
//	auto iter = _animation->_animations[current.animation].boneTransforms.find(skeletion.name);
//	
//	XMMATRIX localTransform = skeletion.local;
//
//	if (iter != _animation->_animations[current.animation].boneTransforms.end())
//	{
//		Animation::BoneTransformTrack& keyFrame = iter->second;
//	
//		XMMATRIX scale = XMMatrixScalingFromVector(InterpolationVector3(keyFrame.scales, current.playTime));
//		XMMATRIX rotation = XMMatrixRotationQuaternion(InterpolationVector4(keyFrame.rotations, current.playTime));
//		XMMATRIX position = XMMatrixTranslationFromVector(InterpolationVector3(keyFrame.positions, current.playTime));
//
//		localTransform = scale * rotation * position;
//	}
//
//	XMMATRIX globalTransform = localTransform * parentTransform;
//
//	if (-1 != skeletion.id)
//	{
//		skeletion.anim = globalTransform;
//		transforms[skeletion.id] = XMMatrixTranspose(skeletion.offset * globalTransform);
//	}
//
//	for (Bone& child : skeletion.children)
//	{
//		UpdateAnimationTransform(child, globalTransform, controllers, transforms);
//	}
//}
//
//XMVECTOR Animator::InterpolationVector3(const std::vector<std::pair<float, Vector3>>& v, const float t)
//{
//	if (v.empty())
//		return XMVectorZero();
//
//	if (v.size() == 1)
//		return v[0].second;
//
//	unsigned int index = FindIndex(v, t);
//	float time0 = v[index].first;
//	float time1 = v[index + 1].first;
//	const Vector3& v1 = v[index].second;
//	const Vector3& v2 = v[index + 1].second;
//
//	float factor = (t - time0) / (time1 - time0);
//
//	return XMVectorLerp(v1, v2, factor);
//}
//
//XMVECTOR Animator::InterpolationVector4(const std::vector<std::pair<float, Vector4>>& v, const float t)
//{
//	if (v.empty())
//		return XMVectorZero();
//
//	if (v.size() == 1)
//		return v[0].second;
//
//	unsigned int index = FindIndex(v, t);
//	float time0 = v[index].first;
//	float time1 = v[index + 1].first;
//	const Vector4& v1 = v[index].second;
//	const Vector4& v2 = v[index + 1].second;
//
//	float factor = (t - time0) / (time1 - time0);
//
//	return XMQuaternionSlerp(v1, v2, factor);
//}
//
//XMMATRIX Animator::BlendAnimation(const Matrix& m0, const Matrix& m1, const float t)
//{
//	XMVECTOR s0, r0, t0;
//	XMVECTOR s1, r1, t1;
//
//	XMMatrixDecompose(&s0, &r0, &t0, XMMatrixTranspose(m0));
//	XMMatrixDecompose(&s1, &r1, &t1, XMMatrixTranspose(m1));
//
//	XMVECTOR blendedScale = XMVectorLerp(s0, s1, t);
//	XMVECTOR blendedRot = XMQuaternionSlerp(r0, r1, t);
//	XMVECTOR blendedPos = XMVectorLerp(t0, t1, t);
//
//	return XMMatrixTranspose(
//		   XMMatrixScalingFromVector(blendedScale) * 
//		   XMMatrixRotationQuaternion(blendedRot) *
//		   XMMatrixTranslationFromVector(blendedPos));
//}
//
//void Animator::BoneMasking(const Bone* bone, int mask)
//{	
//	_boneMask[bone->name] = mask;
//	if (-1 != bone->id) _blendMatrixMask[bone->id] = mask;
//
//	for (auto& child : bone->children)
//		BoneMasking(&child, mask);
//}