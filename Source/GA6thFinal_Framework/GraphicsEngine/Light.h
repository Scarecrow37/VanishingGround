#pragma once
#include "includes/ILight.h"

struct LightData
{
	Vector4 diffuse{};
	Vector4 specular{};
	Vector4 ambient{};	
	Vector3 data;
	float range;
	Vector3 attenuation{};
	float intensity{};
};

class Light : public GE::ILight
{
	friend class DX11Renderer;
public:
	explicit Light();
	virtual ~Light() = default;

public:
	Type GetType() const { return _type; }
	const unsigned int GetID() const { return _ID; }
	bool GetActive() const { return _isActive; }

public:
	// ILight을(를) 통해 상속됨
	void Release() override;
	void SetType(Type type) override;
	void SetIntensity(float intensity) override;
	void SetPosition(float x, float y, float z) override;
	void SetRange(float range) override;
	void SetDirection(float x, float y, float z) override;
	void SetDiffuse(float r, float g, float b, float a) override;
	void SetSpecular(float r, float g, float b, float a) override;
	void SetAmbient(float r, float g, float b, float a) override;
	void SetAttenuation(float constant, float linear, float quadratic) override;
	void SetPositionFromArray(float* pArray) override;
	void SetDirectionFromArray(float* pArray) override;
	void SetDiffuseFromArray(float* pArray) override;
	void SetSpecularFromArray(float* pArray) override;
	void SetAmbientFromArray(float* pArray) override;
	void SetActive(bool isActive) override;

private:
	LightData				_lightData{};
	Type					_type;
	static unsigned int		_globalID;
	unsigned int			_ID{ 0 };
	bool					_isActive{ true };
};