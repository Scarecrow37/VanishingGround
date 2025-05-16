#pragma once

class Camera
{
public:
	const Matrix& GetWorldMatrix() const { return _world; }
	const Matrix& GetViewMatrix() const { return _view; }
	const Matrix& GetProjectionMatrix() const { return _projection; }
    const Matrix& GetProjectionInverseMatrix() const { return _projectionInverse; }

public:
	void SetupPerspective(float fovDegree, float aspect, float nearZ, float farZ);
	void SetRotation(const Vector3& rotation);
	void SetRotation(const Quaternion& rotation);
	void SetPosition(const Vector3& position);
	void AddRotation(const Vector3& rotation);
    void AddRotation(const Quaternion& rotation);
	void AddPosition(const Vector3& position);

public:
	void Update();

private:
    Matrix        _world;
    Matrix        _view;
    Matrix        _projection;
    Matrix        _projectionInverse;
    Vector3       _position;
    Quaternion    _rotation;
};