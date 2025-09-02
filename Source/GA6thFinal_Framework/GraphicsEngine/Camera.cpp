#include "pch.h"
#include "Camera.h"

BoundingFrustum Camera::GetSplitFrustum(float nearZ, float farZ) const
{
    BoundingFrustum frustum;
    frustum.CreateFromMatrix(frustum, _projectionInverse);
    frustum.Near = nearZ;
    frustum.Far  = farZ;

    return frustum;
}

void Camera::SetupPerspective(float fovDegree, float aspect, float nearZ, float farZ)
{
    if (std::isnan(aspect) || std::isinf(aspect))
    {
        aspect = 0.1f;
    }
    else
    {
        aspect = std::max(aspect, 0.1f);
    }
    _projection        = XMMatrixPerspectiveFovLH(XMConvertToRadians(fovDegree), aspect, nearZ, farZ);
    _projectionInverse = XMMatrixInverse(nullptr, _projection);
    _nearZ             = nearZ;
    _farZ              = farZ;

    BoundingFrustum::CreateFromMatrix(_frustum, _projection);
}

void Camera::SetupOrthographic(float width, float height, float nearZ, float farZ)
{
    _projection        = XMMatrixOrthographicLH(width, height, nearZ, farZ);
    _projectionInverse = XMMatrixInverse(nullptr, _projection);
    _nearZ             = nearZ;
    _farZ              = farZ;
}

void Camera::SetWorldMatrix(const Matrix& worldMatrix) 
{
    Matrix matrix = worldMatrix;
    Vector3 scale;
    matrix.Decompose(scale, _rotation, _position);
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
    _prevViewProjection = _view * _projection;
    _world = XMMatrixRotationQuaternion(_rotation) * XMMatrixTranslationFromVector(_position);
    _view  = XMMatrixInverse(nullptr, _world);

    _frustum.Transform(_worldFrustum, _world);

}