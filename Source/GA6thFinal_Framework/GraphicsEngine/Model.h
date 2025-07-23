#pragma once
#include "Resource.h"

class BaseMesh;
class Skeleton;
class Animation;
class Model : public Resource
{
    friend class FBXConverter;

public:
    Model();
    virtual ~Model();

public:
    const std::vector<std::unique_ptr<BaseMesh>>&             GetMeshes() const { return _meshes; }
    const std::vector<std::vector<std::shared_ptr<Texture>>>& GetTextures() const { return _textures; }
    const std::vector<Material>&                              GetMaterials() const { return _material; }
    std::vector<Material>&                                    GetMaterials() { return _material; }
    const std::shared_ptr<Animation>                          GetAnimation() const { return _animation; }
    const std::shared_ptr<Skeleton>                           GetSkeleton() const { return _skeleton; }
    const size_t                                              GetMeshCount() const { return _meshes.size(); }

public:
    void SetMaterial(const UINT meshIndex, const Material& material);

public:
    void AddMesh(std::unique_ptr<BaseMesh> mesh);
    void InitMaterials(UINT materialCount);
    void BindTexture(const UINT meshIndex, std::shared_ptr<Texture> texture);
    void BindMaterial(const UINT meshIndex, const Material& material);

    // Resource을(를) 통해 상속됨
    void LoadResource(const std::filesystem::path& filePath) override;

private:
    std::vector<std::unique_ptr<BaseMesh>>             _meshes;
    std::vector<std::vector<std::shared_ptr<Texture>>> _textures;
    std::vector<Material>                              _material;
    std::shared_ptr<Animation>                         _animation;
    std::shared_ptr<Skeleton>                          _skeleton;
};