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
    float GetMoveSpeed() const { return _moveSpeed; }
    void SetRotationSpeed(const float speed) { _rotationSpeed = speed; }
    float GetRotationSpeed() const { return _rotationSpeed; }

    void SetPosition(const Vector3& position) 
    { 
        _position = position; 
        _pivotPosition = position;
    }
    const Vector3& GetPosition() { return _position; }
    const Vector3& GetPivotPosition() { return _pivotPosition; }

    void SetRotation(const Vector3& rotation) { _rotation = Quaternion::CreateFromYawPitchRoll(rotation); }
    void SetRotation(const Quaternion& rotation) { _rotation = rotation; }
    const Quaternion& GetRotation() { return _rotation; }

    void SetPivot(float value) { _pivot = value; }
    float GetPivot() const { return _pivot; }

    void SetMoveScale(float value) { _moveScale = std::clamp(value, 0.1f, 1000.f); }
    float GetMoveScale() const { return _moveScale; }

public:
    void Update();

private:
    void UpdateMove();
    void UpdateRotate();

private:
    std::shared_ptr<Camera> _camera;
    Vector3                 _position;
    Quaternion              _rotation;
    float                   _moveSpeed;
    float                   _moveScale;
    float                   _rotationSpeed;

    Vector3                 _pivotPosition;
    float                   _pivot;
};
