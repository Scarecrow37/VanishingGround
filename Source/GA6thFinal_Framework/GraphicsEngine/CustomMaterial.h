#pragma once

class MeshRenderer;
class CustomMaterial
{
public:
    CustomMaterial();
    virtual ~CustomMaterial();

public:
    virtual UINT GetRootParameterIndex(MeshType meshType, std::string_view name) const = 0;

public:
    virtual void Initialize() = 0;
    virtual void SetMaterial(ID3D12GraphicsCommandList* commandList, MeshRenderer* renderer) = 0;
};