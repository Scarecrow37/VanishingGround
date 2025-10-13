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
    REFLECT_FIELDS_BEGIN(MeshComponent)
    REFLECT_FIELDS_END(SkeletalMeshRenderer)
    bool _isDirtyFlag = false;
};