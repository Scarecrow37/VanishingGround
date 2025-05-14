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

    Input();
}

void Camera::Input()
{
    //const float deltaTime = UmTime.DeltaTime();
    //const float moveSpeed   = 10.f;
    //const float mouseSpped  = 5.f;
    //const float rotateSpeed = 1.f;

    //if (GetAsyncKeyState('W') & 0x8001)
    //{
    //    _position += -_world.Forward() * moveSpeed * deltaTime;
    //}
    //if (GetAsyncKeyState('S') & 0x8001)
    //{
    //    _position += -_world.Backward() * moveSpeed * deltaTime;
    //}
    //if (GetAsyncKeyState('A') & 0x8001)
    //{
    //    _position += _world.Left() * moveSpeed * deltaTime;
    //}
    //if (GetAsyncKeyState('D') & 0x8001)
    //{
    //    _position += _world.Right() * moveSpeed * deltaTime;
    //}

    //if (GetAsyncKeyState('Q') & 0x8001)
    //{
    //    _position += _world.Up() * moveSpeed * deltaTime;
    //}

    //if (GetAsyncKeyState('E') & 0x8001)
    //{
    //    _position -= _world.Up() * moveSpeed * deltaTime;
    //}    

    //if (GetAsyncKeyState(VK_LEFT) & 0x8001)
    //{
    //    _rotation += Vector3(0.f, -rotateSpeed * deltaTime, 0.f);
    //}

    //if (GetAsyncKeyState(VK_RIGHT) & 0x8001)
    //{
    //    _rotation += Vector3(0.f, rotateSpeed * deltaTime, 0.f);
    //}

    //if (GetAsyncKeyState(VK_UP) & 0x8001)
    //{
    //    _rotation += Vector3(-rotateSpeed * deltaTime, 0.f, 0.f);
    //}

    //if (GetAsyncKeyState(VK_DOWN) & 0x8001)
    //{
    //    _rotation += Vector3(rotateSpeed * deltaTime, 0.f, 0.f);
    //}

    //if (GetAsyncKeyState(VK_RBUTTON) & 0x8001)
    //{
    //    POINT          point;
    //    GetCursorPos((POINT*)&point);

    //    static Vector2 prevPoint{};
    //    static Vector2 currPoint{};
    //    currPoint.x   = point.x;
    //    currPoint.y   = point.y;
    //    Vector2 delta = (currPoint - prevPoint);
    //    delta.Clamp({-1.f, -1.f}, {1.f, 1.f});
    //    prevPoint = currPoint;

    //    if (delta != XMVectorZero())
    //    {
    //        _rotation += Vector3(delta.y * deltaTime * mouseSpped, delta.x * deltaTime * mouseSpped, 0.f);
    //    }

    //    /*ScreenToClient(UmCore->App.GetHwnd(), &point);
    //    SetCursorPos(point.x, point.y);*/
    //}
}