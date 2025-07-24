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
    void Awake() override;
    void Update() override;
    void OnDestroy() override;
    void OnDrawDebug() override;

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

private:
    void LoadModel();
    void OnChangedModel();

public:
    REFLECT_PROPERTY(FilePath)
    GETTER_ONLY(std::string, FilePath) { return _guidRef.ToPath().string(); }
    PROPERTY(FilePath)

private:
    File::GuidRef _guidRef;
    REFLECT_FIELDS_BEGIN(MeshComponent)
    std::string Guid = "";
    REFLECT_FIELDS_END(SkeletalMeshRenderer)
};