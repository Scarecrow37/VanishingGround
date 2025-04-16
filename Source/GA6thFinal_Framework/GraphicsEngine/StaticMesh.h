#pragma once
#include "Model.h"

class StaticMesh : public Model
{
public:
    explicit StaticMesh() = default;
    virtual ~StaticMesh() = default;

public:
    // Model을(를) 통해 상속됨
    HRESULT LoadResource(const std::filesystem::path& filePath) override { return LoadFBX(filePath, true); }
};