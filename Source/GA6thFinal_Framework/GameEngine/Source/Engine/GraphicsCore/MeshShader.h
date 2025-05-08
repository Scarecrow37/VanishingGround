#pragma once
#include "Shader.h"

class MeshShader : public Shader
{
public:
    MeshShader()            = default;
    virtual ~MeshShader() = default;

public:
    // Shader을(를) 통해 상속됨
    HRESULT LoadResource(const std::filesystem::path& filePath) override;
};
