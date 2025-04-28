#pragma once

class Skeleton;
class Transform;
class Animation;
struct Bone;

class Animator
{
	enum TYPE { UPPER, LOWER, END };
    struct Controller
    {
        std::string_view Animation = "";
        float            PlayTime  = 0.f;
        float            LastTime  = 0.f;
        float            Speed     = 1.f;
    };

	struct Blend
    {
        float BlendTime  = 0.f;
        bool  IsBlending = false;
    };

public:
	Animator();
	virtual ~Animator() = default;

public:
	const Matrix* GetAnimationTransform() const { return _animationTransforms.data(); }
	const unsigned int GetID() const { return _ID; }

public:
    void Initialize(std::wstring_view filePath, std::shared_ptr<Skeleton> skeleton);
	void Update(const float deltaTime);

public:
	void Release();
	void ChangeAnimation(const char* animation);
	void ChangeAnimation(const char* animation, unsigned int ID);
	void SyncPartialAnimation(unsigned int parentID, unsigned int childID);
	bool IsLastFrame(float interval, unsigned int ID) const;
	void SetUpSplitBone(unsigned int maxSplit);
	void SplitBone(unsigned int ID, const char* boneName);
	void SetAnimationSpeed(float speed);
	void SetAnimationSpeed(float speed, unsigned int ID);
	void MakeParent(const char* parent, const char* child);

private:
	void UpdateAnimationTransform(Bone& skeletion, const XMMATRIX& parentTransform, std::vector<Controller>& controllers, std::vector<Matrix>& transforms);
	XMVECTOR InterpolationVector3(const std::vector<std::pair<float, Vector3>>& v, const float t);
	XMVECTOR InterpolationVector4(const std::vector<std::pair<float, Vector4>>& v, const float t);
	XMMATRIX BlendAnimation(const Matrix& m0, const Matrix& m1, const float t);

	template<typename T>
	unsigned int FindIndex(T& v, float animationTime)
	{
		unsigned int size = (unsigned int)v.size();
		for (unsigned int i = 0; i < size - 1; i++)
		{
			if (animationTime < v[i + 1].first)
				return i;
		}

		return size - 2;
	}

	void BoneMasking(const Bone* bone, int mask);

private:
	std::vector<Matrix>						_animationTransforms;
	std::vector<Matrix>						_currTransforms;
	std::vector<Matrix>						_prevTransforms;
	std::vector<int>						_blendMatrixMask;

	std::unordered_map<std::string, int>	_boneMask;
	Matrix									_root;
	std::vector<Controller>					_controllers;
	std::vector<Controller>					_prevControllers;
	std::shared_ptr<Animation>				_animation;
	std::vector<Blend>						_blends;
	std::shared_ptr<Skeleton>				_skeleton;
	static unsigned int						_globalID;
	unsigned int							_ID{ 0 };
	unsigned int							_maxSplit{ 0 };	
	bool									_isBlending{ false };
};