#pragma once
#include "GraphicsBase.h"
#include "Interface/ISDFTextRenderer.h"

class SDFFont;
class SDFTextRenderer : public GraphicsBase, public ISDFTextRenderer
{    
public:
    SDFTextRenderer();
    ~SDFTextRenderer();

public:
    // ISDFTextRenderer을(를) 통해 상속됨
    bool    IsActive() const override;
    Vector2 GetStringSize() const override;

    D3D12_GPU_DESCRIPTOR_HANDLE GetFontTextureHandle() const;
    const Vector4&              GetColor() const { return _color; }
    const float                 GetFontWeight() const { return _fontWeight; }
    const SDFFont*              GetFont() const { return _font.get(); }
    const Vector3&              GetRotation() const { return _rotation; }
    const Vector3&              GetPosition() const { return _position; }
    const float                 GetFontSize() const { return _fontSize; }

public:
    void SetFont(std::shared_ptr<SDFFont> font);
    void SetActive(const bool* isActive) override;
    void SetText(const wchar_t* text) override;
    void SetFontSize(const float fontSize) override;
    void SetRotation(const Vector3& rotation) override;
    void SetPosition(const Vector3& position) override;
    void SetColor(const Vector4& color) override;
    void SetFontWeight(const float fontWeight) override;

public:
    void AddReference() override;
    void Release() override;

public:
    void Initialize();

public:
    void Update(ID3D12GraphicsCommandList* commandList);
    void Render(ID3D12GraphicsCommandList* commandList);

private:
    void MeasureString();

private:
    std::vector<Vertex>      _vertices;
    std::wstring             _text;
    std::shared_ptr<SDFFont> _font;
    
    Vector4                  _color;
    Vector3                  _rotation;
    Vector3                  _position;
    Vector2                  _size;
    float                    _fontSize;
    float                    _fontWeight;
    UINT                     _charCount = 0;
    bool                     _dirty = false;

    ComPtr<ID3D12Resource>   _vertexBuffer;
    ComPtr<ID3D12Resource>   _vertexUploadBuffer;
    ComPtr<ID3D12Resource>   _indexBuffer;
    D3D12_VERTEX_BUFFER_VIEW _vertexBufferView;
    D3D12_INDEX_BUFFER_VIEW  _indexBufferView;
};
