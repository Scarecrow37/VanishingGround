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

    void SetPosition(const Vector3& position) { _position = position; }
    const Vector3& GetPosition() { return _position; }

    void SetRotation(const Vector3& rotation) { _rotation = Quaternion::CreateFromYawPitchRoll(rotation); }
    void SetRotation(const Quaternion& rotation) { _rotation = rotation; }
    const Quaternion& GetRotation() { return _rotation; }

public:
    void Update();

private:
    std::shared_ptr<Camera> _camera;
    Vector3                 _position;
    Quaternion              _rotation;
    float                   _moveSpeed;
    float                   _moveScale;
    float                   _rotationSpeed;
};
