#include "pch.h"
#include "Camera.h"

void Camera::Release()
{
	delete this;
}

GE::Matrix4x4 Camera::GetCameraMatrix() const
{
	return _world;
}

void Camera::SetParent(GE::Matrix4x4* pParent)
{
	_pParent = pParent;
}

void Camera::SetPerspective(float nearZ, float farZ, float aspect, float fov)
{
	_projection = XMMatrixPerspectiveFovLH(fov, aspect, nearZ, farZ);
}

void Camera::SetOrthoGraphic(float nearZ, float farZ, float width, float height)
{
	_projection = XMMatrixOrthographicLH(width, height, nearZ, farZ);
}

void Camera::SetPosition(float x, float y, float z)
{
	_position = { x, y, z };
}

void Camera::SetRotationFromAngle(float x, float y, float z)
{
	_rotation = { XMConvertToRadians(x), XMConvertToRadians(y), XMConvertToRadians(z) };
}

void Camera::SetRotationFromRadian(float x, float y, float z)
{
	_rotation = { x, y, z };
}

void Camera::Update()
{
	XMMATRIX rotation = XMMatrixRotationQuaternion(Quaternion::CreateFromYawPitchRoll(_rotation));
	XMMATRIX translation = XMMatrixTranslationFromVector(_position);

	XMMATRIX world = rotation * translation;
	_world = world;

	if (_pParent)
		_world *= (*_pParent);

	_view = XMMatrixInverse(nullptr, _world);
}