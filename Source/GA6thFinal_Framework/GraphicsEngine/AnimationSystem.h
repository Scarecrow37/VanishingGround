#pragma once
#include "includes/IAnimationSystem.h"

class Animator;
class AnimationSystem : public GE::IAnimationSystem
{
	enum class ThreadEvent { None, Process, Destroy, End };
	enum CVEvent { Thread, Done };

public:
	explicit AnimationSystem() = default;
	virtual ~AnimationSystem() = default;

public:
	// IAnimationSystem을(를) 통해 상속됨
	void Initialize(const unsigned int maxThread) override;
	void Update(const float deltaTime) override;
	void CreateAnimator(GE::IMeshRenderer* pMeshRenderer, GE::IAnimator** ppOutAnimator) override;
	void RegisterAnimator(GE::IAnimator* pAnimator) override;
	void UnRegisterAnimator(GE::IAnimator* pAnimator) override;
	void Release() override;

private:
	void WorkerThread(unsigned int index);

private:
	std::condition_variable		_cvDone;
	std::mutex					_mutexDone;
	std::vector<Animator*>		_components;
	std::condition_variable*	_cvs{ nullptr };
	std::mutex*					_mutexes{ nullptr };
	std::thread*				_threads{ nullptr };
	ThreadEvent*				_threadEvents{ nullptr };
	unsigned int				_remainingTasks{ 0 };
	unsigned int				_maxThread{ 0 };
	float						_deltaTime{ 0.f };
	unsigned int				_offset{ 0 };
};