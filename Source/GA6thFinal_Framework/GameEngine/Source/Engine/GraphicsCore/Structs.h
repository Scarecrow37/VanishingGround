#pragma once

struct Vertex
{
	Vector4 Position;
	Vector3 Normal;
	Vector3 Tangent;
	Vector3 BiTangent;
	Vector2 UV;
};

struct StaticMeshVertex
{
	Vector4 Position;
	Vector3 Normal;
	Vector3 Tangent;
	Vector3 BiTangent;
	Vector2 UV;
};

struct SkeletalMeshVertex : public Vertex
{
	UINT BlendIndices[4]{};
	float BlendWeights[4]{};
};

struct ObjectData
{
	XMMATRIX World;
};

struct MaterialData
{
	UINT ID[4];
};

struct Material
{
    #undef OPAQUE
    enum class ShadingModel { UNLIT, DEFAULTLIT, };
    enum class BlendMode{ OPAQUE, MASKED, TRANSLUCENT, ADDITIVE, MODULATE };
    ShadingModel shadingModel;
    BlendMode blendMode;
    bool isTwoSided;
};

struct CameraData
{	
	XMMATRIX View;
	XMMATRIX Projection;
	XMMATRIX ViewInverse;
	XMMATRIX ProejctionInverse;
    Vector4  Position{0.f,0.f,-5.f,1.f};
};