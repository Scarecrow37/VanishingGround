#pragma once
#include "GraphicsBase.h"

struct SkeletalMeshInstance;
class Model;
class Animator;
class MeshRenderer : public GraphicsBase
{
public:
    MeshRenderer(MeshRenderType type, const Matrix& world);
    virtual ~MeshRenderer();

public:
    const std::shared_ptr<Model>& GetModel() const { return _model; }
    const Matrix&                 GetWorldMatrix() const { return _worldMatrix; }
    const MeshRenderType          GetType() const { return _type; }
    const UINT                    GetCustomDepth() const { return _customDepth; }
    std::shared_ptr<Animator>     GetAnimator() const;
    std::vector<std::shared_ptr<SkeletalMeshInstance>>& Get_skeletaMesheInstances() {return _skeletaMesheInstances;}

public:
    void SetModel(std::shared_ptr<Model> model);
    void SetAnimator(std::shared_ptr<Animator> animator);
    void OnCustomDepth(UINT customDepth) { _customDepth |= customDepth; }
    void OffCustomDepth(UINT customDepth) { _customDepth &= ~customDepth; }

public:
    void RegisterRenderQueue(std::string_view sceneName);
    void RegisterRenderQueue();
    void LoadModel(std::wstring_view filePath);

private:
    std::shared_ptr<Model>    _model;
    std::shared_ptr<Animator> _animator;
    const Matrix&             _worldMatrix;
    MeshRenderType            _type;
    UINT                      _customDepth;
    std::vector<std::shared_ptr<SkeletalMeshInstance>> _skeletaMesheInstances;
};