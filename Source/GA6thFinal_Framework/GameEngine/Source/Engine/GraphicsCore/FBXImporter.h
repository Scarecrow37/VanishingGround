#pragma once

class Skeleton;
class Animation;
class Model;
class FBXImporter
{
public:
	FBXImporter();
	~FBXImporter();
public:
	void CreateModel(const std::filesystem::path& filePath, bool isStaticMesh, Model* model);

private:
	void LoadNode(aiNode* node, const aiScene* scene, std::unordered_map<std::string, std::pair<unsigned int, Matrix>>& boneInfo, std::vector<unsigned int>& materialIndex, Model* model);
	void LoadMesh(aiNode* node, aiMesh* mesh, std::unordered_map<std::string, std::pair<unsigned int, Matrix>>& boneInfo, std::vector<unsigned int>& materialIndex, Model* model);
	void LoadMaterials(const aiScene* scene, const std::filesystem::path& filePath, std::vector<std::vector<std::shared_ptr<Texture>>>& textures);
	void FindMissingBone(aiNode* node, std::unordered_map<std::string, std::pair<unsigned int, Matrix>>& boneInfo);
	std::shared_ptr<Texture> LoadTexture(const std::filesystem::path& rootPath, aiString& filePath, aiMaterial* material, aiTextureType type);

private:
	Assimp::Importer _impoter;
	std::shared_ptr<Skeleton> _skeleton;
	std::shared_ptr<Animation> _animation;
	unsigned int _boneCount;
	bool _isStaticMesh;
};