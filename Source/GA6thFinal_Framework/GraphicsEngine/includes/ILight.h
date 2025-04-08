#pragma once
#include "IBash.h"

namespace GE
{
	class ILight : public IBase
	{
	public:
		enum Type { Directional, Point, Spot, End };

	protected:
		explicit ILight() = default;
		virtual ~ILight() = default;
		ILight(const ILight&) = delete;
		ILight(ILight&&) = delete;
		ILight& operator=(const ILight&) = delete;
		ILight& operator=(ILight&&) = delete;

	public:
		virtual void SetType(Type type) = 0;
		virtual void SetIntensity(float intensity) = 0;
		virtual void SetPosition(float x, float y, float z) = 0;
		virtual void SetRange(float range) = 0;
		virtual void SetDirection(float x, float y, float z) = 0;
		virtual void SetDiffuse(float r, float g, float b, float a) = 0;
		virtual void SetSpecular(float r, float g, float b, float a) = 0;
		virtual void SetAmbient(float r, float g, float b, float a) = 0;
		virtual void SetAttenuation(float constant, float linear, float quadratic) = 0;
		virtual void SetPositionFromArray(float* pArray) = 0;
		virtual void SetDirectionFromArray(float* pArray) = 0;
		virtual void SetDiffuseFromArray(float* pArray) = 0;
		virtual void SetSpecularFromArray(float* pArray) = 0;
		virtual void SetAmbientFromArray(float* pArray) = 0;
		virtual void SetActive(bool isActive) = 0;
	};
}