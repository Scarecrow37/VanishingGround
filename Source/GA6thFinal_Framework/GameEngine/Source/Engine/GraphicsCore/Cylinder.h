#pragma once
#include "BaseMesh.h"

class Cylinder : public BaseMesh
{
public:
	Cylinder();
	virtual ~Cylinder();

public:
	void Initialize(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount);
};