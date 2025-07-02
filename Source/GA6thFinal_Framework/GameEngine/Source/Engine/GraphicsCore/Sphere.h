#pragma once
#include "BaseMesh.h"

class Sphere : public BaseMesh
{
public:
	Sphere();
	virtual ~Sphere();

public:
	void Initialize(float radius, uint32_t sliceCount, uint32_t stackCount);
};

