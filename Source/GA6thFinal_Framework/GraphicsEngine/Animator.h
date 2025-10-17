#pragma once
#include "GraphicsBase.h"
#include "Interface/IAnimator.h"

struct Bone;
class Skeleton;
class Animation;
class Animator : public GraphicsBase, public IAnimator
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
	virtual ~Animator();

public:
    void Initialize(std::wstring_view filePath, std::shared_ptr<Skeleton> skeleton);
    void Initialize(std::shared_ptr<Animation> animation, std::shared_ptr<Skeleton> skeleton);
    void Update(const float deltaTime) override;

public:
    bool                            IsActive() const override;
    const Matrix*                   FindBoneMatrix(const char* boneName) const override;
    bool                            IsPaused() const override;
    bool                            IsLoop() const override;
    bool                            IsEnd() const override;
    bool                            IsLastFrame(float interval, unsigned int ID) const override;
    const std::vector<const char*>& GetAnimationNames() const override;
    const Matrix*                   GetAnimationTransform() const { return _animationTransforms.data(); }
    float                           GetAnimationLastTime(const char* animation) const override;
    float                           GetCurrentAnimationLastTime(unsigned int ID = 0) const override;
    float                           GetCurrentAnimationPlayTime(unsigned int ID = 0) const override;
    float                           GetCurrentAnimationSpeed(unsigned int ID = 0) const override;
    bool                            HasAnimation(const char* animation) const override;

public:
    void SetActive(const bool* isActive) override;
    void SetAnimationTime(float time) override;
    void SetAnimationTime(float time, unsigned int ID) override;
    void SetAnimationSpeed(float speed) override;
    void SetAnimationSpeed(float speed, unsigned int ID) override;
    void SetPause(bool isPause) override;
    void SetLoop(bool isLoop) override;
    void SetAnimationEndCallback(std::function<void()> callback) override;

public:
    void AddReference() override;
    void Release() override;

public:
	bool ChangeAnimation(const char* animation, bool blending = true) override;
    bool ChangeAnimation(const char* animation, unsigned int ID, bool blending = true) override;
	void SyncPartialAnimation(unsigned int parentID, unsigned int childID) override;
	void SetUpSplitBone(unsigned int maxSplit) override;
	void SplitBone(unsigned int ID, const char* boneName) override;
	void MakeParent(const char* parent, const char* child) override;

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

    std::unordered_map<std::string, Matrix> _finalBoneMap;
    std::unordered_map<std::string, int>    _boneMask;
    Matrix                                  _root;
    std::vector<Controller>                 _controllers;
    std::vector<Controller>                 _prevControllers;
    std::shared_ptr<Animation>              _animation;
    std::vector<Blend>                      _blends;
    std::shared_ptr<Skeleton>               _skeleton;

	unsigned int							_maxSplit{ 0 };	
	bool                                    _isBlending{false};
    bool                                    _isInitialize{false};
    bool                                    _isPause{false};
    bool                                    _isLoop{true};

    std::function<void()>                   _onAnimationEndCallback{nullptr};
};