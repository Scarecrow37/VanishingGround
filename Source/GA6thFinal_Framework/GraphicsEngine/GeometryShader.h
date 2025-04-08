#pragma once
#include "Shader.h"

class GeometryShader : public Shader
{
	struct Slot
	{
		std::wstring name;
		unsigned int start;
		unsigned int count;
	};
public:
	explicit GeometryShader() = default;
	virtual ~GeometryShader();

public:
	void SetGeometryShader();

public:
	// Resource��(��) ���� ��ӵ�
	HRESULT LoadResource(const std::filesystem::path& filePath) override;

private:
	void Free() override;

private:
	std::vector<Slot>       _inputBindSlot;
	ID3D11GeometryShader*	_pGeometryShader{ nullptr };
};