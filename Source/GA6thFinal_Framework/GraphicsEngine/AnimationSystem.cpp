#include "pch.h"
#include "AnimationSystem.h"
#include "Animator.h"
#include "SkeletalMeshRenderer.h"

void AnimationSystem::Initialize(const unsigned int maxThread)
{
	_maxThread = maxThread;
	_threads = new std::thread[maxThread];
	_cvs = new std::condition_variable[maxThread];
	_mutexes = new std::mutex[maxThread];
	_threadEvents = new ThreadEvent[maxThread];

	for (unsigned int i = 0; i < _maxThread; i++)
	{
		_threads[i] = std::thread(&AnimationSystem::WorkerThread, this, i);
	}
}

void AnimationSystem::Update(const float deltaTime)
{
	_offset = (unsigned int)_components.size() / _maxThread;
	_deltaTime = deltaTime;
	_remainingTasks = _maxThread;
	
	for (unsigned int i = 0; i < _maxThread; i++)
	{
		_threadEvents[i] = ThreadEvent::Process;
		_cvs[i].notify_one();
	}

	std::unique_lock<std::mutex> lock(_mutexDone);
	_cvDone.wait(lock, [this] { return 0 == _remainingTasks; });
}

void AnimationSystem::CreateAnimator(GE::IMeshRenderer* pMeshRenderer, GE::IAnimator** ppOutAnimator)
{
	SkeletalMeshRenderer* pSkeletalMeshRenderer = static_cast<SkeletalMeshRenderer*>(pMeshRenderer);
	ASSERT(MeshType::Skeletal == pSkeletalMeshRenderer->GetType(), L"Not a SkeletalMeshRenderer");
	(*ppOutAnimator) = pSkeletalMeshRenderer->GetAnimator();
}

void AnimationSystem::RegisterAnimator(GE::IAnimator* pAnimator)
{
	auto compare = [pAnimator](const auto* pComponent) 
		{ return static_cast<Animator*>(pAnimator)->GetID() == pComponent->GetID();};

	if (std::ranges::any_of(_components, compare))
	{
		ASSERT(false, L"Already registered Animator.");
		return;
	}

	_components.push_back(static_cast<Animator*>(pAnimator));
}

void AnimationSystem::UnRegisterAnimator(GE::IAnimator* pAnimator)
{
	std::erase_if(_components, [pAnimator](const auto* pComponent)
		{ return static_cast<Animator*>(pAnimator)->GetID() == pComponent->GetID(); });
}

void AnimationSystem::Release()
{	
	for (unsigned int i = 0; i < _maxThread; i++)
		_threadEvents[i] = ThreadEvent::Destroy;

	for (unsigned int i = 0; i < _maxThread; i++)
		_cvs[i].notify_one();

	for (unsigned int i = 0; i < _maxThread; i++)
	{
		_threads[i].join();
	}

	delete[] _threadEvents;
	delete[] _mutexes;
	delete[] _cvs;
	delete[] _threads;
	delete this;
}

void AnimationSystem::WorkerThread(unsigned int index)
{
	bool isLoop = true;
	while (isLoop)
	{
		std::unique_lock<std::mutex> lock(_mutexes[index]);
		_cvs[index].wait(lock, [this, index] { return ThreadEvent::None != _threadEvents[index]; });
		
		switch (_threadEvents[index])
		{
		case ThreadEvent::Process:
		{
			unsigned int start = index * _offset;
			unsigned int end = start + _offset;

			if (index + 1 == _maxThread)
			{
				end += _components.size() % _maxThread;
			}

			for (unsigned int i = start; i < end; i++)
				_components[i]->Update(_deltaTime);

			{
				std::scoped_lock nestLock(_mutexDone);
				_remainingTasks--;

				if (0 == _remainingTasks)
					_cvDone.notify_one();
			}

			_threadEvents[index] = ThreadEvent::None;
			break;
		}
		case ThreadEvent::Destroy:
			isLoop = false;
			break;
		}
	}
}
