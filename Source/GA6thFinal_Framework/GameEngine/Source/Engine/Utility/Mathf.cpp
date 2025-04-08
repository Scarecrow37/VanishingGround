#include "pch.h"

float Mathf::Lerp(float startfloat, float endfloat, float t)
{
	float Lerp = (1.f - t) * startfloat + t * endfloat;
	return Lerp;
}

float Mathf::GetAngleDifference(const DirectX::SimpleMath::Quaternion& q1, const DirectX::SimpleMath::Quaternion& q2)
{
	float dot = q1.Dot(q2);
	dot = std::clamp(dot, -1.0f, 1.0f);
	float angleDifference = 2.0f * std::acos(std::abs(dot));
	return angleDifference;
}
