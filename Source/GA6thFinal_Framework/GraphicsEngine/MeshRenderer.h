#pragma once
#include "GraphicsBase.h"

class Model;
class Animator;
class DXRSkeletalMesh;

class MeshRenderer : public GraphicsBase
{
public:
    MeshRenderer(MeshType type, const Vector3& position, const Vector3& scale, const Quaternion& rotation, const Matrix& world);
    virtual ~MeshRenderer();

public:
    const std::shared_ptr<Model>& GetModel() const { return _model; }
    const Matrix&                 GetWorldMatrix() const { return _transform.World; }
    const GraphicsTransform&      GetTransform() const { return _transform; }
    const MeshType                GetType() const { return _type; }
    const UINT                    GetCustomDepth(UINT meshID) const { return _customDepths[meshID]; }
    const std::vector<UINT>&      GetCustomDepths() const { return _customDepths; }
    std::shared_ptr<Animator>     GetAnimator() const;
    const std::vector<std::shared_ptr<DXRSkeletalMesh>> GetDXRSkeletalMeshes() const { return _dxrSkeletalMeshes; }

public:
    void SetModel(std::shared_ptr<Model> model);
    void SetAnimator(std::shared_ptr<Animator> animator);
    void SetMaterial(const UINT meshIndex, const Material& material);
    void SetMasterMaterial(const UINT meshIndex, const Material& material);

public:
    void OnCustomDepth(UINT customDepth);
    void OnCustomDepth(UINT customDepth, UINT meshID);    
    void OffCustomDepth(UINT customDepth);
    void OffCustomDepth(UINT customDepth, UINT meshID);

private:
    std::vector<Material>                              _materials;
    std::vector<UINT>                                  _customDepths;
    std::vector<std::shared_ptr<DXRSkeletalMesh>>      _dxrSkeletalMeshes;
    std::shared_ptr<Model>                             _model;
    std::shared_ptr<Animator>                          _animator;
    GraphicsTransform                                  _transform;
    MeshType                                           _type;
};