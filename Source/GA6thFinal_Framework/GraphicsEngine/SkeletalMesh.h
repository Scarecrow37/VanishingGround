#pragma once
#include "Model.h"

class SkeletalMesh : public Model
{
public:
    explicit SkeletalMesh() = default;
    virtual ~SkeletalMesh() = default;

public:
    // Model��(��) ���� ��ӵ�
    HRESULT LoadResource(const std::filesystem::path& filePath) override { return LoadFBX(filePath, false); }
};