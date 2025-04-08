#pragma once
#include "Shader.h"

class VertexShader : public Shader
{
    friend class MeshRenderer;
    friend class SkinnedMeshRenderer;

    struct Slot
    {
        std::wstring name;
        unsigned int start;
        unsigned int count;
    };
public:
    explicit VertexShader() = default;
    virtual ~VertexShader();

public:
    void SetVertexShader();

public:
    // Resource을(를) 통해 상속됨
    HRESULT LoadResource(const std::filesystem::path& filePath) override;

private:
    void Free() override;

private:
    std::vector<Slot>       _inputBindSlot;
    ID3D11VertexShader*     _pVertexShader{ nullptr };
    ID3D11InputLayout*      _pInputLayout{ nullptr };
    unsigned int            _vertexBufferStride{ 0 };
    unsigned int            _vertexBufferOffset{ 0 };
};