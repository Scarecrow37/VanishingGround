#pragma once
#include "Interface/ISDFTextRenderer.h"

class SDFFont;
class SDFTextRenderer : public ISDFTextRenderer
{
    friend class RenderScene;

public:
    SDFTextRenderer();
    ~SDFTextRenderer();

public:
    // ISDFTextRenderer을(를) 통해 상속됨
    bool                        IsActive() const override;
    D3D12_GPU_DESCRIPTOR_HANDLE GetFontTextureHandle() const;
    const Vector4&              GetColor() const { return _color; }
    const Vector2&              GetSize() const { return _size; }
    const Matrix&               GetWorldMatrix() const;

public:
    void SetFont(std::shared_ptr<SDFFont> font);
    void SetActive(const bool* isActive) override;
    void SetText(const wchar_t* text) override;
    void SetPosition(const Vector3& position) override;
    void SetScale(const float scale) override;
    void SetColor(const Vector4& color) override;
    void SetRotation(const float rotation) override;

public:
    void Release() override;

public:
    void Initialize() override;

public:
    void Update(ID3D12GraphicsCommandList* commandList);
    void Render(ID3D12GraphicsCommandList* commandList);

private:
    std::vector<bool*> _isDestroyeds;
    const bool*        _isActive{nullptr};

private:
    std::vector<Vertex>      _vertices;
    std::wstring             _text;
    std::shared_ptr<SDFFont> _font;
    Vector4                  _color;
    Vector3                  _position;
    Vector2                  _size;
    float                    _scale;
    float                    _rotation;
    Matrix                   _worldMatrix;
    bool                     _dirty = false;

    ComPtr<ID3D12Resource>   _vertexBuffer;
    ComPtr<ID3D12Resource>   _vertexUploadBuffer;
    ComPtr<ID3D12Resource>   _indexBuffer;
    D3D12_VERTEX_BUFFER_VIEW _vertexBufferView;
    D3D12_INDEX_BUFFER_VIEW  _indexBufferView;
};
