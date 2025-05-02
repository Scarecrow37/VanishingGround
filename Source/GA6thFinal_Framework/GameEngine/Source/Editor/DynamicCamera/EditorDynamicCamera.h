#pragma once

class Camera;
class EditorDynamicCamera
{
public:
    EditorDynamicCamera();
    ~EditorDynamicCamera() = default;

public:
    const std::shared_ptr<Camera>& GetCamera() const { return _camera; }

public:
    void SetTarget(std::shared_ptr<Camera> camera);
    void SetMoveSpeed(const float speed) { _moveSpeed = speed; }
    void SetRotationSpeed(const float speed) { _rotationSpeed = speed; }

public:
    void Update();

private:
    std::shared_ptr<Camera> _camera;
    Vector3                 _position;
    Vector3                 _rotation;
    float                   _moveSpeed;
    float                   _rotationSpeed;
};
