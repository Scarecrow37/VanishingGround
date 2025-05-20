#pragma once
#include "UmFramework.h"
#include "MeshComponent.h"

class StaticMeshRenderer : public MeshComponent
{
    USING_PROPERTY(StaticMeshRenderer)

public:
    StaticMeshRenderer();
    virtual ~StaticMeshRenderer();

public:
    REFLECT_PROPERTY(FilePath)
    GETTER_ONLY(std::string, FilePath)
    {
        if (nullptr != _guidRef)
        {
            return _guidRef->ToPath().string();
        }
        else
        {
            return File::NULL_PATH.string();
        }
    }
    PROPERTY(FilePath)

public:
    std::shared_ptr<File::Guid> _guidRef;

    REFLECT_FIELDS_BEGIN(Component)
    std::string Guid;
    REFLECT_FIELDS_END(StaticMeshRenderer)

protected:
    virtual void Reset() override;
    virtual void Awake() override;
    virtual void Start() override;
    virtual void OnEnable() override;
    virtual void OnDisable() override;
    virtual void Update() override;
    virtual void FixedUpdate() override;
    virtual void OnDestroy() override;
    virtual void OnApplicationQuit() override;

    virtual void SerializedReflectEvent() override;
    virtual void DeserializedReflectEvent() override;
};