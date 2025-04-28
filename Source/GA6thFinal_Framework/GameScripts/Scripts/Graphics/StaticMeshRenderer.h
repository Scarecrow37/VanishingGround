#pragma once
#include "UmFramework.h"
#include "MeshRenderer.h"

class Model;
class BaseMesh;
class StaticMeshRenderer : public MeshRenderer
{
    USING_PROPERTY(StaticMeshRenderer)

public:
    StaticMeshRenderer();
    virtual ~StaticMeshRenderer();

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