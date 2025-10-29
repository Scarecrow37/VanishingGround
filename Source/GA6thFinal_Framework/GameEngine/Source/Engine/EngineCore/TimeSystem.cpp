#include "pch.h"
using namespace Global;

ETimeSystem::ETimeSystem()
{
	QueryPerformanceFrequency(&_frequency);
	QueryPerformanceCounter(&_previousTime);
	QueryPerformanceCounter(&_currentTime);
}
ETimeSystem::~ETimeSystem() = default;

void ETimeSystem::TimeSystemUpdate()
{
	++_frameCount;

	_previousTime = _currentTime;
	QueryPerformanceCounter(&_currentTime);
	LONGLONG tickTime = _currentTime.QuadPart - _previousTime.QuadPart;

	_realtimeSinceStartup += tickTime;
	_time += tickTime * (LONGLONG)TimeScale;

	_unscaledDeltaTime = (std::min)(double(tickTime) / double(_frequency.QuadPart), MaximumDeltaTime);
	_deltaTime = _unscaledDeltaTime * TimeScale;

	if (FixedTimeStep > std::numeric_limits<double>::epsilon())
	{
		_elapsedFixedTime += _deltaTime;
	}

    UpdateInvokeFunctions();
}

bool ETimeSystem::TimeSystemFixedUpdate()
{
	if (_elapsedFixedTime >= FixedTimeStep && FixedTimeStep > std::numeric_limits<double>::epsilon())
	{
		_fixedDeltaTime = _elapsedFixedTime;
		_fixedUnscaledDeltaTime = _elapsedFixedTime / TimeScale;
		_elapsedFixedTime -= FixedTimeStep;
		return true;
	}
	return false;
}

void ETimeSystem::UpdateInvokeFunctions() 
{
    bool erase = false;
    float deltaTime = (float)_deltaTime;
    for (auto& [weak, delay, func, elapsed] : _safeInvokeFunctions)
    {
        if (true == weak.expired())
        {
            erase = true;
        }
        else
        {
            elapsed += deltaTime;
            if (delay <= elapsed)
            {
                func();
                erase = true;
            }
        }
    }
    if (erase)
    {
        std::erase_if(_safeInvokeFunctions, [](auto& tuple) 
        { 
            auto& [weak, delay, func, elapsed] = tuple;
            return weak.expired() || delay <= elapsed;
        });
    }

    erase = false;
    for (auto& [delay, func, elapsed] : _unsafeInvokeFunctions)
    {
        elapsed += deltaTime;
        if (delay <= elapsed)
        {
            func();
            erase = true;
        }
    }
    if (erase)
    {
        std::erase_if(_unsafeInvokeFunctions, [](auto& tuple) 
        {
            auto& [delay, func, elapsed] = tuple;
            return delay <= elapsed;
        });
    }
}

void ETimeSystem::Engine::TimeSystemUpdate()
{
	engineCore->Time.TimeSystemUpdate();
}

bool ETimeSystem::Engine::TimeSystemFixedUpdate()
{
	return engineCore->Time.TimeSystemFixedUpdate();
}

void ETimeSystem::Engine::CleanUpInvokeFuntions() 
{
    engineCore->Time._unsafeInvokeFunctions.clear();
    engineCore->Time._safeInvokeFunctions.clear();
}
