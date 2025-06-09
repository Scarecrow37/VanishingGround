#pragma once
#include "VIBuffer.h"

class BaseMesh
{
public:
    BaseMesh();
    virtual ~BaseMesh();

public:
    std::string_view GetName() const { return _name; }
    void             SetName(std::string_view name) { _name = name; }
    void             GetVertexInfo(char*& vertices, unsigned int& stride, unsigned int& size);

public:
    void Initialize(const VIBuffer::Descriptor& descriptor, bool createVertexInfo = false);
    void Render(ID3D12GraphicsCommandList* commandList);

private:
    std::string               _name;
    std::unique_ptr<VIBuffer> _viBuffer;
    char*                     _vertices;
    unsigned int              _vertexStride;
    unsigned int              _vertexSize;
};