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
        return _guidRef.ToPath().string();
    }
    PROPERTY(FilePath)
private:
    void LoadModel();

public:
    File::GuidRef _guidRef;

    REFLECT_FIELDS_BEGIN(Component)
    std::string Guid;
    REFLECT_FIELDS_END(StaticMeshRenderer)

protected:
    virtual void SerializedReflectEvent() override;
    virtual void DeserializedReflectEvent() override;
    virtual void Reset() override;
};