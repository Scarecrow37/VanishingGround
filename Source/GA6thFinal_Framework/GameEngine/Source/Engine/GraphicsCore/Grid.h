#pragma once
#include "BaseMesh.h"

class Grid : public BaseMesh
{
public:
	Grid();
	virtual ~Grid();

public:
	void Initialize(float width, float depth, uint32_t m, uint32_t n);
};