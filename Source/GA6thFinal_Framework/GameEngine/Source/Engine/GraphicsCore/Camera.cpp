#include "pch.h"
#include "Camera.h"

void Camera::SetupPerspective(float fovDegree, float aspect, float nearZ, float farZ)
{
    _projection        = XMMatrixPerspectiveFovLH(XMConvertToRadians(fovDegree), aspect, nearZ, farZ);
    _projectionInverse = XMMatrixInverse(nullptr, _projection);
}

void Camera::SetRotation(const Vector3& rotation)
{
    _rotation = Quaternion::CreateFromYawPitchRoll(rotation);
}

void Camera::SetRotation(const Quaternion& rotation) 
{
    _rotation = rotation;
}

void Camera::SetPosition(const Vector3& position)
{
	_position = position;
}

void Camera::AddRotation(const Vector3& rotation)
{
    _rotation *= Quaternion::CreateFromYawPitchRoll(rotation);
}

void Camera::AddRotation(const Quaternion& rotation) 
{
    _rotation *= rotation;
}

void Camera::AddPosition(const Vector3& position)
{
	_position += position;
}

void Camera::Update()
{
    _world = XMMatrixRotationQuaternion(_rotation) * XMMatrixTranslationFromVector(_position);
    _view  = XMMatrixInverse(nullptr, _world);
}