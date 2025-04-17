#pragma once
#include "Resource.h"

class BaseMesh;
class Skeleton;
class Animation;
class Model : public Resource
{
public:
	Model();
	virtual ~Model();

public:
	const std::vector<std::unique_ptr<BaseMesh>>& GetMeshes() { return _meshes; }
	const std::vector<std::vector<std::shared_ptr<Texture>>>& GetMaterials() { return _materials; }

public:
	void AddMesh(std::unique_ptr<BaseMesh> mesh);
	void InitMaterials(UINT materialCount) { _materials.resize(materialCount); }
	void BindMaterial(const UINT meshIndex, std::shared_ptr<Texture> texture);

	// Resource을(를) 통해 상속됨
	HRESULT LoadResource(const std::filesystem::path& filePath) override;

private:
	std::vector<std::unique_ptr<BaseMesh>>	_meshes;
	std::vector<std::vector<std::shared_ptr<Texture>>>	_materials;
	std::shared_ptr<Animation>				_animation;
	std::unique_ptr<Skeleton>				_skeleton;
};