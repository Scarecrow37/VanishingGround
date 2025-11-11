#pragma once
#include "VIBuffer.h"

class BaseMesh
{
public:
    BaseMesh();
    virtual ~BaseMesh();

public:
    const BoundingOrientedBox& GetBoundingBox() const { return _boundingBox; }
    std::string_view           GetName() const { return _name; }
    void                       SetName(std::string_view name) { _name = name; }
    void                       GetVertexInfo(char*& vertices, unsigned int& stride, unsigned int& size);
    VIBuffer*                  GetVIBuffer() { return _viBuffer.get(); }
    bool                       IsCullingEnabled() const { return _isCullingEnabled; }

public:
    void SetCullingEnabled(bool enabled) { _isCullingEnabled = enabled; }

public:
    void Initialize(const VIBuffer::Descriptor& descriptor, bool createVertexInfo = false);
    void Initialize(ID3D12GraphicsCommandList* commandList, const VIBuffer::Descriptor& descriptor, bool createVertexInfo = false);
    void Render(ID3D12GraphicsCommandList* commandList);
    void Render(ID3D12GraphicsCommandList* commandList , UINT instanceCount);

private:
    std::string               _name;
    BoundingOrientedBox       _boundingBox;
    std::unique_ptr<VIBuffer> _viBuffer;
    char*                     _vertices;
    char*                     _indices;
    unsigned int              _vertexStride;
    unsigned int              _vertexSize;
    bool                      _isCullingEnabled{true};
};