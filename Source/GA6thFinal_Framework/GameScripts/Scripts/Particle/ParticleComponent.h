#pragma once
    
class ParticleComponent : public Component
{
    USING_PROPERTY(ParticleComponent)
public:

    REFLECT_PROPERTY(FilePath, Position, Rotation, Scale, AttachToBoneMatrix)
    GETTER_ONLY(std::string, FilePath) { return _filepath.string(); }
    PROPERTY(FilePath)

    GETTER(const Vector3&, Position) { return _positionVector; }
    SETTER(const Vector3&, Position)
    {
        _positionVector = value;
        std::memcpy(&ReflectFields->PositionArray[0], &_positionVector.x, sizeof(ReflectFields->PositionArray));
    }
    PROPERTY(Position)

    GETTER(const Vector3&, Rotation) { return _rotationVector; }
    SETTER(const Vector3&, Rotation) 
    { 
        _rotationVector = value;
        std::memcpy(&ReflectFields->RotationArray[0], &_rotationVector.x, sizeof(ReflectFields->RotationArray));
    }
    PROPERTY(Rotation)

    GETTER(const Vector3&, Scale) { return _scaleVector; }
    SETTER(const Vector3&, Scale)
    {
        _scaleVector = value;
        std::memcpy(&ReflectFields->ScaleArray[0], &_scaleVector.x, sizeof(ReflectFields->ScaleArray));
    }
    PROPERTY(Scale)

    GETTER(bool, AttachToBoneMatrix) { return ReflectFields->AttachToBoneMatrix; }
    SETTER(bool, AttachToBoneMatrix) { 
        ReflectFields->AttachToBoneMatrix = value;
        FollowBoneMatrix();
    }
    PROPERTY(AttachToBoneMatrix)

    GETTER_ONLY(const ParticleEffect*, Effect) { return _effect; }
    PROPERTY(Effect)

    void PlayEffect();
    void StopEffect();
    void SetGuid(const File::Path& filepath);
    void SetGuid(const File::Guid& fileguid);
    void FollowBoneMatrix();

public:
    ParticleComponent();
    virtual ~ParticleComponent();

    File::GuidRef _guidRef;
    File::Path    _filepath;

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::string          Guid;
    std::array<float, 3> PositionArray;
    std::array<float, 3> RotationArray;
    std::array<float, 3> ScaleArray;
    bool                 AttachToBoneMatrix;
    std::string          BoneNameToAttach;
    REFLECT_FIELDS_END(ParticleComponent)

    ParticleEffect* _effect;

    void            Update() override;
    void            Start() override;
    void            Reset() override;
    void            OnDestroy() override;

    void            SerializedReflectEvent() override;
    void            DeserializedReflectEvent() override;
    void            ImGuiDrawPropertysEvent() override;
    void            Awake() override;
    void            OnEnable() override;

 private:
    void  LoadParticle();
    
    class SkeletalMeshRenderer* _skelMesh;
    Vector3 _positionVector{0.f, .0f, 0.f};
    Vector3 _rotationVector{0.f, 0.f, 0.f};
    Vector3 _scaleVector{1.f, 1.f, 1.f};
};
