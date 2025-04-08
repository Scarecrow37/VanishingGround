#pragma once
#include "includes/ICamera.h"

class Camera : public GE::ICamera
{
public:
	explicit Camera() = default;
	virtual ~Camera() = default;

public:
	const Matrix& GetViewMatrix() const { return _view; }
	const Matrix& GetProjectionMatrix() const { return _projection; }
	Vector3 GetPosition() const { return _world.Translation(); }

public:
	// ICamera을(를) 통해 상속됨
	void Release() override;
	GE::Matrix4x4 GetCameraMatrix() const;
	void SetParent(GE::Matrix4x4* pParent) override;
	void SetPerspective(float nearZ, float farZ, float aspect, float fov) override;
	void SetOrthoGraphic(float nearZ, float farZ, float width, float height) override;
	void SetPosition(float x, float y, float z) override;
	void SetRotationFromAngle(float x, float y, float z) override;
	void SetRotationFromRadian(float x, float y, float z) override;

public:
	void Update();

private:
	Matrix	_world;
	Matrix	_view;
	Matrix	_projection;
	Vector3	_position;
	Vector3	_rotation;
	Matrix* _pParent{ nullptr };
};