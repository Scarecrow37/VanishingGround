#pragma once
#include "Resource.h"

class Shader : public Resource
{
protected:
	explicit Shader();
	virtual ~Shader() = default;

protected:
	void CompileShader(std::vector<uint8_t>& output, const std::filesystem::path& filePath, const char* entry, const char* model);

protected:
	ID3D11DeviceContext* _pDeviceContext{ nullptr };
};