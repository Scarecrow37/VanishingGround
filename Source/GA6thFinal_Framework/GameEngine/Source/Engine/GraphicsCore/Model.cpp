#include "pch.h"
#include "Model.h"
#include "Skeleton.h"
#include "Animation.h"
#include "BaseMesh.h"
#include "FBXImporter.h"

Model::Model()
{
}

Model::~Model()
{
}

void Model::AddMesh(std::unique_ptr<BaseMesh> mesh)
{
	_meshes.emplace_back(std::move(mesh));
}

void Model::InitMaterials(UINT materialCount)
{
    _textures.resize(materialCount);
    _material.resize(materialCount);
}

void Model::BindTexture(const UINT meshIndex, std::shared_ptr<Texture> texture)
{
	_textures[meshIndex].emplace_back(texture);
}

void Model::BindMaterial(const UINT meshIndex, const Material& material)
{
    _material[meshIndex] = material;
}

HRESULT Model::LoadResource(const std::filesystem::path& filePath)
{
    FBXImporter fbxImporter;
    fbxImporter.CreateModel(filePath, true, this);
    
	return S_OK;
}
