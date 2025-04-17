#pragma once
#include "Resource.h"

class Texture : public Resource
{
public:
	Texture() = default;
	virtual ~Texture() = default;

public:
	// Resource��(��) ���� ��ӵ�
	HRESULT LoadResource(const std::filesystem::path& filePath) override;
};

