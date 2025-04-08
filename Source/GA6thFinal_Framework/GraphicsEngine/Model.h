#pragma once
#include "Resource.h"

class Mesh;
class Texture;
class Skeleton;
class Animation;
class VertexShader;
class Model : public Resource
{
	friend class DX11Renderer;
	friend class SkyBoxRenderer;
public:
	explicit Model() = default;
	virtual ~Model();

public:
	Skeleton* GetSkeleton() const { return _pSkeleton; }
	void BindVertexShader(std::shared_ptr<VertexShader>& vertexShader);

protected:
	HRESULT LoadFBX(const std::filesystem::path& filePath, bool isStaticMesh);

private:
	void LoadNode(aiNode* paiNode, const aiScene* paiScene, std::unordered_map<std::string, std::pair<unsigned int, Matrix>>& boneInfo, std::vector<unsigned int>& materialIndex);
	void LoadMesh(aiNode* paiNode, aiMesh* paiMesh, std::unordered_map<std::string, std::pair<unsigned int, Matrix>>& boneInfo, std::vector<unsigned int>& materialIndex);
	void LoadMaterials(const aiScene* paiScene, const std::filesystem::path& filePath, std::vector<std::vector<std::shared_ptr<Texture>>>& textures);
	void FindMissingBone(aiNode* paiNode, std::unordered_map<std::string, std::pair<unsigned int, Matrix>>& boneInfo);
	std::shared_ptr<Texture> LoadTexture(const std::filesystem::path& rootPath, const aiScene* paiScene, aiString& filePath, aiMaterial* pMaterial, aiTextureType type, const wchar_t* format);

private:
	void Free() override;

private:
	std::vector<Mesh*>			_meshs;
	std::shared_ptr<Animation>	_animation;
	Skeleton*					_pSkeleton = nullptr;
	unsigned int				_boneCount = 0;
};