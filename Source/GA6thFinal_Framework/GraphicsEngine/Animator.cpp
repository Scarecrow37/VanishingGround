#include "pch.h"
#include "Animator.h"
#include "Skeleton.h"
#include "Animation.h"

Animator::Animator()
	: _maxSplit(1)
{
}

Animator::~Animator()
{   
    Global::animationCore->UnregisterAnimator(this);
}

const Matrix* Animator::FindBoneMatrix(const char* boneName) const
{
    auto iter = _finalBoneMap.find(boneName);

    if (iter != _finalBoneMap.end())
    {
        return &iter->second;
    }

    return nullptr;
}

float Animator::GetCurrentAnimationLastTime(unsigned int ID) const
{
    if (ID >= _controllers.size())
    {
        GRAPHICS_ASSERT(false, L"Greater than the number of controllers you set.");
        return 0.0f;
    }

    if (false == _controllers.empty())
    {
        return _controllers[ID].LastTime;
    }
    return 0.0f;
}

float Animator::GetAnimationLastTime(const char* animation) const
{
    auto iter = _animation->_animations.find(animation);
    if (iter != _animation->_animations.end())
    {
        return iter->second.LastTime;
    }
    return 0.0f;
}

std::string_view Animator::GetCurrentAnimationName(unsigned int ID) const
{
    if (ID >= _controllers.size())
    {
        GRAPHICS_ASSERT(false, L"Greater than the number of controllers you set.");
        return "";
    }
    return _controllers[ID].Animation;
}

float Animator::GetCurrentAnimationPlayTime(unsigned int ID) const
{
    if (ID >= _controllers.size())
    {
        GRAPHICS_ASSERT(false, L"Greater than the number of controllers you set.");
        return 0.0f;
    }
    return _controllers[ID].PlayTime;
}

float Animator::GetCurrentAnimationSpeed(unsigned int ID) const
{
    if (ID >= _controllers.size())
    {
        GRAPHICS_ASSERT(false, L"Greater than the number of controllers you set.");
        return 0.0f;
    }
    return _controllers[ID].Speed;
}

bool Animator::HasAnimation(const char* animation) const
{
    auto iter = _animation->_animations.find(animation);
    if (iter != _animation->_animations.end())
    {
        return true;
    }
    return false;
}

void Animator::SetActive(const bool* isActive)
{
    GraphicsBase::SetActive(isActive);
}

bool Animator::IsPaused() const
{
    return _isPause;
}

bool Animator::IsLoop() const
{
    return _isLoop;
}

bool Animator::IsEnd() const
{
    size_t count = 0;
    for (size_t i = 0; i < _maxSplit; i++)
    {
        if (_controllers[i].PlayTime >= _controllers[i].LastTime)
        {
            ++count;
        }
    }
    if (count == _maxSplit)
    {
        return true;
    }
    return false;
}

void Animator::SetAnimationTime(float time) 
{
    for (unsigned int i = 0; i < _maxSplit; i++)
    {
        _controllers[i].PlayTime = time;
    }
}

void Animator::SetAnimationTime(float time, unsigned int ID)
{
    if (ID >= _controllers.size())
    {
        GRAPHICS_ASSERT(false, L"Greater than the number of controllers you set.");
        return;
    }

    if (false == _controllers.empty())
    {
        _controllers[0].PlayTime = time;
    }
}

void Animator::SetAnimationSpeed(float speed)
{
    for (unsigned int i = 0; i < _maxSplit; i++)
    {
        _controllers[i].Speed = speed;
    }
}

void Animator::SetAnimationSpeed(float speed, unsigned int ID)
{
    if (_maxSplit <= ID)
    {
        GRAPHICS_ASSERT(false, L"Greater than the number of bones you set.");
        return;
    }

    _controllers[ID].Speed = speed;
}

void Animator::SetPause(bool isPause)
{
    _isPause = isPause;
}

void Animator::SetLoop(bool isLoop) 
{
    _isLoop = isLoop;
}

void Animator::SetAnimationEndCallback(std::function<void()> callback) 
{
    _onAnimationEndCallback = callback;
}

void Animator::AddReference()
{
    //GraphicsBase::AddReference();
}

void Animator::Release()
{
    //GraphicsBase::Release();
}

const std::vector<const char*>& Animator::GetAnimationNames() const
{
    if (_animation)
    {
        return _animation->GetAnimations();
    }
    static std::vector<const char*> emptyNames;
    return emptyNames;
}

void Animator::Initialize(std::wstring_view filePath, std::shared_ptr<Skeleton> skeleton)
{
	_animation = Global::resourceManager->LoadResource<Animation>(filePath);
    _skeleton  = skeleton;
    InitializeFinalBoneMap();
	_controllers.resize(1);
	_prevControllers.resize(1);
	_blends.resize(1);

	_animationTransforms.resize(MAX_BONE_MATRIX);
	_currTransforms.resize(MAX_BONE_MATRIX);
	_prevTransforms.resize(MAX_BONE_MATRIX);
	_blendMatrixMask.resize(MAX_BONE_MATRIX);

    _isInitialize = true;
}

void Animator::Initialize(std::shared_ptr<Animation> animation, std::shared_ptr<Skeleton> skeleton)
{
    if (nullptr == animation || nullptr == skeleton)
        return;

    _animation = animation;
    _skeleton  = skeleton;
    InitializeFinalBoneMap();
    _controllers.resize(1);
    _prevControllers.resize(1);
    _blends.resize(1);

    _animationTransforms.resize(MAX_BONE_MATRIX);
    _currTransforms.resize(MAX_BONE_MATRIX);
    _prevTransforms.resize(MAX_BONE_MATRIX);
    _blendMatrixMask.resize(MAX_BONE_MATRIX);

    _isInitialize = true;
}

void Animator::Update(const float deltaTime)
{
    if (!_isInitialize)
        return;

	XMMATRIX identity = XMMatrixIdentity();

	for (unsigned int i = 0; i < _maxSplit; i++)
	{
		const Animation::Channel& animation = _animation->_animations[_controllers[i].Animation.data()];
        if (false == _isPause && _controllers[i].PlayTime < animation.LastTime)
        {
		    _controllers[i].PlayTime += _controllers[i].Speed * deltaTime;
        }
	
		if (_controllers[i].PlayTime > animation.LastTime)
		{
            if (true == _isLoop)
            {
                bool isDevByZero = (0.0f == animation.LastTime); 
                _controllers[i].PlayTime = isDevByZero ? 0.0f : fmod(_controllers[i].PlayTime, animation.LastTime);
            }
            else
            {
                _controllers[i].PlayTime = animation.LastTime;
                _isPause                 = true;
            }
            if (_onAnimationEndCallback)
            {
                _onAnimationEndCallback();
            }
		}
	}

	UpdateAnimationTransform(_skeleton->GetRootBone(), identity, _controllers, _animationTransforms);

	if (_isBlending)
	{
		memcpy(_currTransforms.data(), _animationTransforms.data(), sizeof(Matrix) * MAX_BONE_MATRIX);
		UpdateAnimationTransform(_skeleton->GetRootBone(), identity, _prevControllers, _prevTransforms);

		unsigned int count = 0;
		for (unsigned int i = 0; i < _maxSplit; i++)
		{
			if (_blends[i].IsBlending)
			{
				if (1.f <= _blends[i].BlendTime)
				{
                    _blends[i].BlendTime  = 0.f;
                    _blends[i].IsBlending = false;
				}
				else
				{
					float t = 1 - (1 - _blends[i].BlendTime) * (1 - _blends[i].BlendTime);

					for (size_t j = 0; j < MAX_BONE_MATRIX; j++)
					{
						if (i != _blendMatrixMask[j])
						{
							//_animationTransforms[j] = _currTransforms[j];
						}
						else
						{
							_animationTransforms[j] = BlendAnimation(_prevTransforms[j], _currTransforms[j], t);
						}
					}

					_blends[i].BlendTime += deltaTime * 5.f;
				}
			}

			if (!_blends[i].IsBlending)
				count++;
		}

		if (count == _maxSplit)
			_isBlending = false;
	}
}

bool Animator::IsActive() const
{
    return GraphicsBase::IsActive();
}

bool Animator::ChangeAnimation(const char* animation, bool blending)
{
    int count = 0;

    for (unsigned int i = 0; i < _maxSplit; i++)
    {
        if (ChangeAnimation(animation, i, blending))
        {
            ++count;
        }
    }

    return count > 0;
}

bool Animator::ChangeAnimation(const char* animation, const unsigned int ID, bool blending)
{
	auto iter = _animation->_animations.find(animation);
	if (iter == _animation->_animations.end())
		return false;

	//if (!strcmp(_controllers[ID].Animation.data(), animation))
	//	return false;

	_isBlending = true;
    _blends[ID].BlendTime  = 0.f;
    _blends[ID].IsBlending = true;
    if (true == blending)
    {
        _prevControllers[ID] = _controllers[ID];
    }
    _controllers[ID].Animation = iter->first;
    _controllers[ID].PlayTime  = 0.f;
    _controllers[ID].LastTime  = iter->second.LastTime;
    if (false == blending)
    {
        _prevControllers[ID] = _controllers[ID];
    }

    return true;
}

void Animator::SyncPartialAnimation(unsigned int parentID, unsigned int childID)
{
	if (parentID >= _maxSplit || childID >= _maxSplit)
		return;

	_isBlending = true;
	_blends[childID].BlendTime = 0.f;
	_blends[childID].IsBlending = true;

	_prevControllers[childID] = _controllers[childID];
	//_prevControllers[parentID] = _controllers[parentID];
	_controllers[childID] = _controllers[parentID];
}

bool Animator::IsLastFrame(float interval, const unsigned int ID) const
{
	return _controllers[ID].PlayTime + interval > _controllers[ID].LastTime;
}

void Animator::SetUpSplitBone(const unsigned int maxSplit)
{
	_skeleton->SetUpSplitBone(maxSplit);
	_maxSplit = maxSplit;

	_controllers.resize(maxSplit);
	_prevControllers.resize(maxSplit);
	_blends.resize(maxSplit);
}

void Animator::SplitBone(const unsigned int ID, const char* boneName)
{
	if (_maxSplit <= ID)
	{
        GRAPHICS_ASSERT(false, L"Greater than the number of bones you set.");
		return;
	}

	_skeleton->SplitBone(ID, boneName);
	BoneMasking(_skeleton->GetBone(ID), ID);
}

void Animator::MakeParent(const char* parent, const char* child)
{
	_skeleton->MakeParent(parent, child);
}

void Animator::InitializeFinalBoneMap() 
{
    _finalBoneMap.clear();
    Bone&             rootBone = _skeleton->GetRootBone();
    TraverseBoneMap(rootBone);
}
void Animator::TraverseBoneMap(const Bone& bone) 
{
    _finalBoneMap[bone.Name] = XMMatrixIdentity();
    for (const auto& child : bone.Children)
    {
        TraverseBoneMap(child);
    }
}

//void Animator::GetSkeletonMatrix(const char* bone, GE::Matrix4x4** out)
//{
//	Bone& rootBone = _pSkeleton->GetRootBone();
//
//	std::queue<Bone*> bfs;
//	bfs.push(&rootBone);
//
//	while (!bfs.empty())
//	{
//		Bone* curr = bfs.front();
//		bfs.pop();
//
//		if (curr->name == bone)
//		{
//			(*out) = (GE::Matrix4x4*)&curr->anim;
//			break;
//		}
//		for (auto& child : curr->children)
//		{
//			bfs.push(&child);
//		}
//	}
//}

void Animator::UpdateAnimationTransform(Bone& skeletion, 
										const XMMATRIX& parentTransform, 
										std::vector<Controller>& controllers, 
										std::vector<Matrix>& transforms)
{
	Controller& current = controllers[_boneMask[skeletion.Name]];
	auto iter = _animation->_animations[current.Animation.data()].BoneTransforms.find(skeletion.Name);
	
	XMMATRIX localTransform = skeletion.Local;

	if (iter != _animation->_animations[current.Animation.data()].BoneTransforms.end())
	{
		Animation::BoneTransformTrack& keyFrame = iter->second;
	
		XMMATRIX scale = XMMatrixScalingFromVector(InterpolationVector3(keyFrame.Scales, current.PlayTime));
		XMMATRIX rotation = XMMatrixRotationQuaternion(InterpolationVector4(keyFrame.Rotations, current.PlayTime));
		XMMATRIX position = XMMatrixTranslationFromVector(InterpolationVector3(keyFrame.Positions, current.PlayTime));

		localTransform = scale * rotation * position;
	}

	XMMATRIX globalTransform = localTransform * parentTransform;

	if (-1 != skeletion.ID)
	{
        _finalBoneMap[skeletion.Name] = globalTransform;
        transforms[skeletion.ID]      = XMMatrixTranspose(skeletion.Offset * globalTransform);
	}

	for (Bone& child : skeletion.Children)
	{
		UpdateAnimationTransform(child, globalTransform, controllers, transforms);
	}
}

XMVECTOR Animator::InterpolationVector3(const std::vector<std::pair<float, Vector3>>& v, const float t)
{
	if (v.empty())
		return XMVectorZero();

	if (v.size() == 1)
		return v[0].second;

	unsigned int index = FindIndex(v, t);
	float time0 = v[index].first;
	float time1 = v[index + 1].first;
	const Vector3& v1 = v[index].second;
	const Vector3& v2 = v[index + 1].second;

	float factor = (t - time0) / (time1 - time0);

	return XMVectorLerp(v1, v2, factor);
}

XMVECTOR Animator::InterpolationVector4(const std::vector<std::pair<float, Vector4>>& v, const float t)
{
	if (v.empty())
		return XMVectorZero();

	if (v.size() == 1)
		return v[0].second;

	unsigned int index = FindIndex(v, t);
	float time0 = v[index].first;
	float time1 = v[index + 1].first;
	const Vector4& v1 = v[index].second;
	const Vector4& v2 = v[index + 1].second;

	float factor = (t - time0) / (time1 - time0);

	return XMQuaternionSlerp(v1, v2, factor);
}

XMMATRIX Animator::BlendAnimation(const Matrix& m0, const Matrix& m1, const float t)
{
	XMVECTOR s0, r0, t0;
	XMVECTOR s1, r1, t1;

	XMMatrixDecompose(&s0, &r0, &t0, XMMatrixTranspose(m0));
	XMMatrixDecompose(&s1, &r1, &t1, XMMatrixTranspose(m1));

	XMVECTOR blendedScale = XMVectorLerp(s0, s1, t);
	XMVECTOR blendedRot = XMQuaternionSlerp(r0, r1, t);
	XMVECTOR blendedPos = XMVectorLerp(t0, t1, t);

	return XMMatrixTranspose(
		   XMMatrixScalingFromVector(blendedScale) * 
		   XMMatrixRotationQuaternion(blendedRot) *
		   XMMatrixTranslationFromVector(blendedPos));
}

void Animator::BoneMasking(const Bone* bone, int mask)
{	
	_boneMask[bone->Name] = mask;
	if (-1 != bone->ID) _blendMatrixMask[bone->ID] = mask;

	for (auto& child : bone->Children)
		BoneMasking(&child, mask);
}