#pragma once
#include "BaseMesh.h"

class GeoSphere : public BaseMesh
{
public:
	GeoSphere();
	virtual ~GeoSphere();

public:
	void Initialize(float radius, uint32_t numSubdivisions);
};