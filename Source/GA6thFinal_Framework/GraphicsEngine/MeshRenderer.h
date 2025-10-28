#pragma once
#include "GraphicsBase.h"
#include "Interface/IMeshRenderer.h"

class Model;
class Animator;
class DXRSkeletalMesh;
class CustomMaterial;
class MeshRenderer : public GraphicsBase, public IMeshRenderer
{
public:
    MeshRenderer();
    virtual ~MeshRenderer();

public:
    bool                     IsActive() const override { return GraphicsBase::IsActive(); }
    const UINT               GetCustomDepth(UINT meshID) const override { return _customDepths[meshID]; }
    const std::vector<UINT>& GetCustomDepths() override;
    IAnimator*               GetAnimator() const override;
    std::shared_ptr<Model>   GetModel() const override { return _model; }
    const MeshType           GetType() const override { return _type; }
    const CustomLightType    GetCustomLightType() const override { return _customLightType; }
    std::vector<Material>&   GetMaterials() override { return _materials; }    

    const Matrix&                                       GetWorldMatrix() const { return *_world; }
    const std::vector<std::shared_ptr<DXRSkeletalMesh>> GetDXRSkeletalMeshes() const { return _dxrSkeletalMeshes; }
    const std::any&                                     GetCustomMaterialData() const { return _customMaterialData; }

public:
    void SetActive(const bool* isActive) override;
    void SetMaterial(const UINT meshIndex, const Material& material) override;
    void SetMasterMaterial(const UINT meshIndex, const Material& material) override;
    void SetCustomMaterial(CustomLightType type, const std::any& customMaterial) override;

    void SetModel(std::shared_ptr<Model> model);

public:
    void AddReference() override;
    void Release() override;

public:
    void OnCustomDepth(UINT customDepth) override;
    void OnCustomDepth(UINT customDepth, UINT meshID) override;
    void OffCustomDepth(UINT customDepth) override;
    void OffCustomDepth(UINT customDepth, UINT meshID) override;

public:
    void Initialize(const Matrix* world);

private:
    std::vector<Material>                         _materials;
    std::vector<UINT>                             _customDepths;
    std::vector<std::shared_ptr<DXRSkeletalMesh>> _dxrSkeletalMeshes;
    std::shared_ptr<Model>                        _model;
    std::unique_ptr<Animator>                     _animator;
    const Matrix*                                 _world{nullptr};
    MeshType                                      _type{MeshType::MESH_TYPE_END};
    std::any                                      _customMaterialData;
    CustomLightType                               _customLightType{CustomLightType::NONE};
};