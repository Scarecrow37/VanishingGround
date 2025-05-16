#pragma once
#include "RendererBase.h"

class Model;
class MeshRenderer : public RendererBase
{
public:
    enum class RENDER_TYPE { STATIC, SKELETAL, };

public:
    MeshRenderer(RENDER_TYPE type, const Matrix& world);
    virtual ~MeshRenderer();

public:
    const std::shared_ptr<Model>& GetModel() const { return _model; }
    const Matrix&                 GetWorldMatrix() const { return _worldMatrix; }
    RENDER_TYPE                   GetType() const { return _type; }

public:    
    void SetModel(std::shared_ptr<Model> model);

public:
    void RegisterRenderQueue(std::string_view sceneName) override;
    void LoadModel(std::wstring_view filePath);

private:
    std::shared_ptr<Model> _model;
    const Matrix&          _worldMatrix;
    RENDER_TYPE            _type;
};