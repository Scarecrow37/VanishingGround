#pragma once
#include "Model.h"

class SkeletalMesh : public Model
{
public:
    explicit SkeletalMesh() = default;
    virtual ~SkeletalMesh() = default;

public:
    // Model을(를) 통해 상속됨
    HRESULT LoadResource(const std::filesystem::path& filePath) override { return LoadFBX(filePath, false); }
};