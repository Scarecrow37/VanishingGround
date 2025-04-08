#include "pch.h"
#include "Light.h"

unsigned int Light::_globalID = 0;

Light::Light()
	: _ID(_globalID++)
{
}

void Light::Release()
{
	delete this;
}

void Light::SetType(Type type)
{
	_type = type;
}

void Light::SetIntensity(float intensity)
{
	_lightData.intensity = intensity;
}

void Light::SetPosition(float x, float y, float z)
{
	_lightData.data = { x, y, z };
}

void Light::SetRange(float range)
{
	_lightData.range = range;
}

void Light::SetDirection(float x, float y, float z)
{
	_lightData.data = { x, y, z };
}

void Light::SetDiffuse(float r, float g, float b, float a)
{
	_lightData.diffuse = { r, g, b, a };
}

void Light::SetSpecular(float r, float g, float b, float a)
{
	_lightData.specular = { r, g, b, a };
}

void Light::SetAmbient(float r, float g, float b, float a)
{
	_lightData.ambient = { r, g, b, a };
}

void Light::SetAttenuation(float constant, float linear, float quadratic)
{
	_lightData.attenuation = { constant, linear, quadratic };
}

void Light::SetPositionFromArray(float* pArray)
{
	_lightData.data = Vector3(pArray);
}

void Light::SetDirectionFromArray(float* pArray)
{
	_lightData.data = Vector3(pArray);
}

void Light::SetDiffuseFromArray(float* pArray)
{
	_lightData.diffuse = Vector4(pArray);
}

void Light::SetSpecularFromArray(float* pArray)
{
	_lightData.specular = Vector4(pArray);
}

void Light::SetAmbientFromArray(float* pArray)
{
	_lightData.ambient = Vector4(pArray);
}

void Light::SetActive(bool isActive)
{
	_isActive = isActive;
}
