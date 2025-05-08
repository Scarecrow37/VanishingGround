#pragma once
#include "Shader.h"

class ComputeShader : public Shader
{
public:
    ComputeShader()         = default;
    virtual ~ComputeShader() = default;

public:
    // Shader을(를) 통해 상속됨
    HRESULT LoadResource(const std::filesystem::path& filePath) override;
};
