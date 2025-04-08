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

	_unscaledDeltaTime = double(tickTime) / double(_frequency.QuadPart);
	_deltaTime			= (std::min)(_unscaledDeltaTime * TimeScale, MaximumDeltaTime);

	if (FixedTimeStep > std::numeric_limits<double>::epsilon())
	{
		_elapsedFixedTime += _deltaTime;
	}
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

void ETimeSystem::Engine::TimeSystemUpdate()
{
	engineCore->Time.TimeSystemUpdate();
}

bool ETimeSystem::Engine::TimeSystemFixedUpdate()
{
	return engineCore->Time.TimeSystemFixedUpdate();
}
