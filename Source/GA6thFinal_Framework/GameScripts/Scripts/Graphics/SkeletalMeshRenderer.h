#pragma once
#include "MeshRenderer.h"

class Animation;
class Animator;
class SkeletalMeshRenderer : public MeshRenderer
{
public:
    SkeletalMeshRenderer();
    virtual ~SkeletalMeshRenderer();

public:
    std::shared_ptr<Animator> GetAnimator() const { return _animator; }

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
    
private:
    std::shared_ptr<Animator> _animator;
};
