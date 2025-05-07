#pragma once
#include "Shader.h"

class GeometryShader : public Shader
{
public:
    GeometryShader()        = default;
    virtual ~GeometryShader() = default;

public:
    // Shader을(를) 통해 상속됨
    HRESULT LoadResource(const std::filesystem::path& filePath) override;
};
