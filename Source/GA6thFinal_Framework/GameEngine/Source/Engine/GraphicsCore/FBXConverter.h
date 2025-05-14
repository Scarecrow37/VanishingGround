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
    void ImportModel(const std::filesystem::path& filePath, Model* model);
    void ExportModel(const std::filesystem::path& filePath);

private:
    void LoadNode(aiNode* node, const aiScene* scene, std::unordered_map<std::string, std::pair<unsigned int, Matrix>>& boneInfo, Model* model);
    void LoadMesh(aiNode* node, aiMesh* mesh, std::unordered_map<std::string, std::pair<unsigned int, Matrix>>& boneInfo, Model* model);
    void LoadMaterials(const aiScene* scene, const std::filesystem::path& filePath);
    void FindMissingBone(aiNode* node, std::unordered_map<std::string, std::pair<unsigned int, Matrix>>& boneInfo);
    std::string ConvertPath(aiString& filePath, aiMaterial* material, aiTextureType type);

    void Reset();
    void LoadFromAssimp(const std::filesystem::path& filePath, Model* model);
    void LoadFromBinary(const std::filesystem::path& filePath, Model* model);

private:    
    std::vector<std::vector<StaticMeshVertex>>   _staticVertices;
    std::vector<std::vector<SkeletalMeshVertex>> _skeletalVertices;
    std::vector<std::vector<unsigned int>>       _indices;
    std::vector<std::vector<std::string>>        _textures;
    std::vector<unsigned int>                    _materialIndex;

    std::shared_ptr<Skeleton>                    _skeleton;
    std::shared_ptr<Animation>                   _animation;
    unsigned int                                 _boneCount;
    bool                                         _isStaticMesh;
};