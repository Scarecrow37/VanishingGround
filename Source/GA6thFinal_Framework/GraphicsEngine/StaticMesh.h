#pragma once
#include "Model.h"

class StaticMesh : public Model
{
public:
    explicit StaticMesh() = default;
    virtual ~StaticMesh() = default;

public:
    // Model��(��) ���� ��ӵ�
    HRESULT LoadResource(const std::filesystem::path& filePath) override { return LoadFBX(filePath, true); }
};