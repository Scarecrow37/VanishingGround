#pragma once
#include "MeshComponent.h"

class StaticMeshRenderer : public MeshComponent
{
    USING_PROPERTY(StaticMeshRenderer)

public:
    StaticMeshRenderer();
    virtual ~StaticMeshRenderer() override;

public:
    REFLECT_PROPERTY(FilePath)
    GETTER_ONLY(std::string, FilePath)
    { 
        return _Guid.ToPath().string();
    }
    PROPERTY(FilePath)
private:
    void LoadModel();

public:
    File::Guid _Guid;

    REFLECT_FIELDS_BEGIN(MeshComponent)
    REFLECT_FIELDS_END(StaticMeshRenderer)

protected:
    void DeserializedReflectEvent() override;
    void Reset() override;
    void OnDrawDebugSelected() override;
    void OnDrawDebug() override;
};