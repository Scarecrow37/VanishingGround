#pragma once
#include "BaseMesh.h"

class Quad : public BaseMesh
{
public:
	Quad();
	virtual ~Quad();

public:
	void Initialize(float x, float y, float w, float h, float depth);
};