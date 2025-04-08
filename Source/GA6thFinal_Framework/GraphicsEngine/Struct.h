#pragma once

struct ViewProjection
{
	XMMATRIX vp;
	XMMATRIX shadowVP;	
};

struct CameraDesc
{
	XMMATRIX vpInvers;
	XMVECTOR cameraPosition;
};