#pragma once

class Model;
class MeshRenderer
{
public:
    enum class RENDER_TYPE { STATIC, SKELETAL, };

public:
    MeshRenderer(RENDER_TYPE type, const Matrix& world);
    ~MeshRenderer();

public:
    const std::shared_ptr<Model>& GetModel() const { return _model; }
    const Matrix&                 GetWorldMatrix() const { return _worldMatrix; }
    RENDER_TYPE                   GetType() const { return _type; }
    bool                          IsActive() const { return _isActive; }
    bool                          IsDestroy() const { return _isDestroy; }

public:
    void SetActive(bool isActive) { _isActive = isActive; }
    void SetDestroy() { _isDestroy = true; }

public:
    void RegisterRenderQueue(std::string_view sceneName);
    void LoadModel(std::wstring_view filePath);

private:
    std::shared_ptr<Model> _model;
    const Matrix&          _worldMatrix;
    RENDER_TYPE            _type;
    bool                   _isActive;
    bool                   _isDestroy;
};