#pragma once
#include "IBash.h"

namespace GE
{
	class ICamera : public IBase
	{
	protected:
		explicit ICamera() = default;
		virtual ~ICamera() = default;
		ICamera(const ICamera&) = delete;
		ICamera(ICamera&&) = delete;
		ICamera& operator=(const ICamera&) = delete;
		ICamera& operator=(ICamera&&) = delete;

	public:
		virtual Matrix4x4 GetCameraMatrix() const = 0;
		virtual void SetPerspective(float nearZ, float farZ, float aspect, float fov) = 0;
		virtual void SetOrthoGraphic(float nearZ, float farZ, float width, float height) = 0;
		virtual void SetPosition(float x, float y, float z) = 0;
		virtual void SetRotationFromAngle(float x, float y, float z) = 0;
		virtual void SetRotationFromRadian(float x, float y, float z) = 0;
		virtual void SetParent(Matrix4x4* pParent) = 0;
	};
}