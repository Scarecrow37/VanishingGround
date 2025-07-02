#pragma once
#include "BaseMesh.h"

class Box : public BaseMesh
{
public:
	Box();
	virtual ~Box();

public:
	void Initialize(float width, float height, float depth, uint32_t numSubdivisions = 0);
    void InitializeInverted(float width, float height, float depth, uint32_t numSubdivisions = 0);
};

