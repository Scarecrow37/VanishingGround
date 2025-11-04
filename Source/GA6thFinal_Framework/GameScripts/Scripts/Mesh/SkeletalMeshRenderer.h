#pragma once
#include "MeshComponent.h"
#include "Animation/Structs/AnimationData.h"

class SkeletalMeshRenderer : public MeshComponent
{
    USING_PROPERTY(SkeletalMeshRenderer)

public:
    SkeletalMeshRenderer();
    virtual ~SkeletalMeshRenderer() override;

public:
    const Matrix* GetBoneMatrixFormMappingKey(const std::string& key);

protected:
    void Reset() override;    

    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

private:
    void LoadModel();
    void OnChangedModel();

public:
    REFLECT_PROPERTY(FilePath)
    GETTER_ONLY(std::string, FilePath) { return _Guid.ToPath().string(); }
    PROPERTY(FilePath)

private:
    File::Guid _Guid;
    std::unordered_map<std::string, const Matrix*> _boneMatrixMappingTable;
    REFLECT_FIELDS_BEGIN(MeshComponent)
    std::unordered_map<std::string, std::string> BoneKeyMap;
    REFLECT_FIELDS_END(SkeletalMeshRenderer)
    bool _isDirtyFlag = false;
};