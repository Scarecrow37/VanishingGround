#pragma once
#include "Base.h"

class Resource abstract : public Base
{
protected:
	explicit Resource() = default;
	virtual ~Resource() = default;

public:
	virtual HRESULT LoadResource(const std::filesystem::path& filePath) = 0;
};