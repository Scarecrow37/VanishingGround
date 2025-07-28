#pragma once

class Camera
{
public:
	const Matrix& GetWorldMatrix() const { return _world; }
	const Matrix& GetViewMatrix() const { return _view; }
	const Matrix& GetProjectionMatrix() const { return _projection; }
    const Matrix& GetProjectionInverseMatrix() const { return _projectionInverse; }
    const Vector3 GetPosition() const { return _world.Translation(); }
    const Quaternion& GetRotation() const { return _rotation; }
    const BoundingFrustum& GetWorldFrustum() const { return _worldFrustum; }

public:
	void SetupPerspective(float fovDegree, float aspect, float nearZ, float farZ);
    void SetupOrthographic(float width, float height, float nearZ, float farZ);
    void SetWorldMatrix(const Matrix& worldMatrix);
	void SetRotation(const Vector3& rotation);
	void SetRotation(const Quaternion& rotation);
	void SetPosition(const Vector3& position);
	void AddRotation(const Vector3& rotation);
    void AddRotation(const Quaternion& rotation);
	void AddPosition(const Vector3& position);

public:
	void Update();

private:
    Matrix          _world;
    Matrix          _view;
    Matrix          _projection;
    Matrix          _projectionInverse;
    BoundingFrustum _frustum;
    BoundingFrustum _worldFrustum;
    Vector3         _position;
    Quaternion      _rotation;
};