#pragma once
#include "MeshRenderer.h"

class Model;
class StaticMeshRenderer : public MeshRenderer
{
public:
	explicit StaticMeshRenderer() = default;
	virtual ~StaticMeshRenderer() = default;

public:
	void Initialize(const wchar_t* filePath) override;
};