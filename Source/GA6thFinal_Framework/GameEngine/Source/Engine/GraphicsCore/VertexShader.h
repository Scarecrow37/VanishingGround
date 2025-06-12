#pragma once
#include "Shader.h"

class VertexShader : public Shader
{
public:
    VertexShader() = default;
    virtual ~VertexShader() = default;

public:
    // Shader을(를) 통해 상속됨
    void LoadResource(const std::filesystem::path& filePath) override;
};
