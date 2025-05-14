#pragma once

class Skeleton;
class Animation;
class Model;
class FBXConverter
{
public:
    FBXConverter();
    ~FBXConverter();

public:
    void CreateModel(Model* model);
    void ImportFBX(const std::filesystem::path& filePath);
    void ExportFBX(const std::filesystem::path& filePath);

private:
    void LoadNode(aiNode* node, const aiScene* scene, std::unordered_map<std::string, std::pair<unsigned int, Matrix>>& boneInfo, std::vector<unsigned int>& materialIndex);
    void LoadMesh(aiNode* node, aiMesh* mesh, std::unordered_map<std::string, std::pair<unsigned int, Matrix>>& boneInfo, std::vector<unsigned int>& materialIndex);
    void LoadMaterials(const aiScene* scene, const std::filesystem::path& filePath);
    void FindMissingBone(aiNode* node, std::unordered_map<std::string, std::pair<unsigned int, Matrix>>& boneInfo);
    std::shared_ptr<Texture> LoadTexture(const std::filesystem::path& rootPath, aiString& filePath, aiMaterial* material, aiTextureType type);
    std::string ConvertPath(const std::filesystem::path& rootPath, aiString& filePath, aiMaterial* material, aiTextureType type);

    void LoadFromAssimp(const std::filesystem::path& filePath);
    void LoadFromBinary(const std::filesystem::path& filePath);

private:    
    std::vector<std::vector<StaticMeshVertex>>   _staticVertices;
    std::vector<std::vector<SkeletalMeshVertex>> _skeletalVertices;
    std::vector<std::vector<unsigned int>>       _indices;
    std::vector<std::vector<std::string>>        _textures;

    std::shared_ptr<Skeleton>                    _skeleton;
    std::shared_ptr<Animation>                   _animation;
    unsigned int                                 _boneCount;
    bool                                         _isStaticMesh;
};