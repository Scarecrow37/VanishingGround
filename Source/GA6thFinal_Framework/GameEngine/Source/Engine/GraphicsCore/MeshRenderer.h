#pragma once
#include "GraphicsBase.h"

class Model;
class Animator;
class MeshRenderer : public GraphicsBase
{
public:
    MeshRenderer(MESH_RENDER_TYPE type, const Matrix& world);
    virtual ~MeshRenderer();

public:
    const std::shared_ptr<Model>& GetModel() const { return _model; }
    const Matrix&                 GetWorldMatrix() const { return _worldMatrix; }
    const MESH_RENDER_TYPE        GetType() const { return _type; }
    std::shared_ptr<Animator>     GetAnimator() const;

public:
    void SetModel(std::shared_ptr<Model> model);
    void SetAnimator(std::shared_ptr<Animator> animator);

public:
    void RegisterRenderQueue(std::string_view sceneName);
    void LoadModel(std::wstring_view filePath);

private:
    std::shared_ptr<Model>    _model;
    std::shared_ptr<Animator> _animator;
    const Matrix&             _worldMatrix;
    MESH_RENDER_TYPE          _type;
};