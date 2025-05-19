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

public:
    void Initialize(const VIBuffer::Descriptor& descriptor);
    void Render(ID3D12GraphicsCommandList* commandList);

private:
    std::string               _name;
    std::unique_ptr<VIBuffer> _viBuffer;
};