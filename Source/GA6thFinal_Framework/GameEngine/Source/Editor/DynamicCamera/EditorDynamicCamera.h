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
    void SetMoveSpeed(const float speed) { _moveSpeed = std::clamp(speed, _minmaxMoveSpeed.first, _minmaxMoveSpeed.second); }
    float GetMoveSpeed() const { return _moveSpeed; }
    void SetRotationSpeed(const float speed) { _rotationSpeed = std::clamp(speed, _minmaxRotationSpeed.first, _minmaxRotationSpeed.second); }
    float GetRotationSpeed() const { return _rotationSpeed; }

    void SetPosition(const Vector3& position) {  _position = position;  _pivotPosition = position; }
    void SetPivotPosition(const Vector3& position) { _pivotPosition = position; }
    const Vector3& GetPosition() { return _position; }
    const Vector3& GetPivotPosition() { return _pivotPosition; }

    void SetRotation(const Vector3& rotation) { _rotation = Quaternion::CreateFromYawPitchRoll(rotation); }
    void SetRotation(const Quaternion& rotation) { _rotation = rotation; }
    const Quaternion& GetRotation() { return _rotation; }

    void SetPivot(float value) { _pivot = value; }
    float GetPivot() const { return _pivot; }

    bool IsManipulated() const { return _isManipulated; }
    bool IsMoved() const { return _isMoved; }
    bool IsRotated() const { return _isRotated; }

    void SetMinMoveSpeed(float min) { _minmaxMoveSpeed.first = min; }  
    void SetMinRotationSpeed(float min) { _minmaxRotationSpeed.first = min; }
    void SetMaxMoveSpeed(float max) { _minmaxMoveSpeed.second = max; }
    void SetMaxRotationSpeed(float max) { _minmaxRotationSpeed.second = max; }
    float GetMinMoveSpeed() const { return _minmaxMoveSpeed.first; }
    float GetMinRotationSpeed() const { return _minmaxRotationSpeed.first; }
    float GetMaxMoveSpeed() const { return _minmaxMoveSpeed.second; }
    float GetMaxRotationSpeed() const { return _minmaxRotationSpeed.second; }

public:
    void Update();

private:
    // 움직인 경우 true, 움직이지 않은 경우 false
    bool UpdateMove();
    // 회전한 경우 true, 회전하지 않은 경우 false
    bool UpdateRotate();

private:
    std::shared_ptr<Camera> _camera;
    Vector3                 _position;
    Quaternion              _rotation;
    std::pair<float, float> _minmaxMoveSpeed;
    float                   _moveSpeed;
    std::pair<float, float> _minmaxRotationSpeed;
    float                   _rotationSpeed;

    Vector3                 _pivotPosition;
    float                   _pivot;

    bool                    _isMoved;
    bool                    _isRotated;
    bool                    _isManipulated;
};
