#pragma once
    
class ParticleComponent : public Component
{
    using EffectID = int;
    using EffectCallback = std::function<void(void)>;
    USING_PROPERTY(ParticleComponent)
public:

    REFLECT_PROPERTY(EffectName, FilePath, Position, Rotation, Scale, AttachToBoneMatrix)

    GETTER_ONLY(std::string, FilePath) { return _filepath.string(); }
    PROPERTY(FilePath)

    GETTER(std::string, EffectName) { return _newEffectKey; }
    SETTER(std::string, EffectName) { _newEffectKey = value; }
    PROPERTY(EffectName)

    GETTER(const Vector3&, Position)
    {
        auto it = _positionVector.find(_currentEffectKey);
        if (it != _positionVector.end())
        {
            return (*it).second;
        }
        else
            return _zeroVec;
    }
    SETTER(const Vector3&, Position)
    {
        auto it = _positionVector.find(_currentEffectKey);
        if (it != _positionVector.end())
        {
            (*it).second = value;
            std::memcpy(&ReflectFields->TranslationMap[_currentEffectKey][0], &(*it).second.x,
                        sizeof(std::array<float, 3>));
        }
    }
    PROPERTY(Position)

    GETTER(const Vector3&, Rotation)
    {
        auto it = _rotationVector.find(_currentEffectKey);
        if (it != _rotationVector.end())
        {
            return (*it).second;
        }
        else
            return _zeroVec;
    }
    SETTER(const Vector3&, Rotation)
    {
        auto it = _rotationVector.find(_currentEffectKey);
        if (it != _rotationVector.end())
        {
            (*it).second = value;
            std::memcpy(&ReflectFields->RotationMap[_currentEffectKey][0], &(*it).second.x,
                        sizeof(std::array<float, 3>));
        }
    }
    PROPERTY(Rotation)

    GETTER(const Vector3&, Scale)
    {
        auto it = _scaleVector.find(_currentEffectKey);
        if (it != _scaleVector.end())
        {
            return (*it).second;
        }
        else
            return _zeroVec;
    }
    SETTER(const Vector3&, Scale)
    {
        auto it = _scaleVector.find(_currentEffectKey);
        if (it != _scaleVector.end())
        {
            (*it).second = value;
            std::memcpy(&ReflectFields->ScaleMap[_currentEffectKey][0], &(*it).second.x,
                        sizeof(std::array<float, 3>));
        }
    }
    PROPERTY(Scale)

    GETTER(bool, AttachToBoneMatrix) 
    { 
        auto it = ReflectFields->AttachFlagMap.find(_currentEffectKey);
        if (it != ReflectFields->AttachFlagMap.end())
        {
            return (*it).second;
        }
        else
            return false;
    }
    SETTER(bool, AttachToBoneMatrix)
    {
        auto it = ReflectFields->AttachFlagMap.find(_currentEffectKey);
        if (it != ReflectFields->AttachFlagMap.end())
        {
            (*it).second = value;
            //FollowBoneMatrix(_currentEffectKey);
        }
    }
    PROPERTY(AttachToBoneMatrix)

    void PlayEffect(const std::string& key);
    void PlayEffect(const std::string& key, EffectCallback callback);
    void StopEffect(const std::string& key);
    void StopAll();
    void ClearEffectList();
    void DeleteEffect(const std::string& key);
    void RegisterEffectFromGuid(const File::Path& filepath, const std::string& key);
    void RegisterEffectFromGuid(const File::Guid& fileguid, const std::string& key);
    void FollowBoneMatrix(const std::string& key);
    void FollowBoneMatrix();
    void SetAnimator(class Animator* animator);

public:
    ParticleComponent();
    virtual ~ParticleComponent();

    File::Path    _filepath;
protected:
    REFLECT_FIELDS_BEGIN(Component)

    std::unordered_map<std::string,std::array<float, 3>> ScaleMap;
    std::unordered_map<std::string,std::array<float, 3>> RotationMap;
    std::unordered_map<std::string,std::array<float, 3>> TranslationMap;
    std::unordered_map<std::string,std::string>          GuidMap;
    std::unordered_map<std::string,std::string>          BoneNameMap;
    std::unordered_map<std::string,bool>                 AttachFlagMap;
    std::vector<std::string> EffectNameTable;

    REFLECT_FIELDS_END(ParticleComponent)

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;
    void Start() override;


  void Update() override;

private:
    void LoadParticle(const std::string& keyString);
    
    class SkeletalMeshRenderer* _skelMesh;
    std::unordered_map<std::string,Vector3> _positionVector;
    std::unordered_map<std::string,Vector3> _rotationVector;
    std::unordered_map<std::string,Vector3> _scaleVector;
    

    std::string _currentEffectKey = "-";
    std::string _newEffectKey     = "";
    Vector3     _zeroVec          = {0, 0, 0};

    EffectID _objectInstanceID = -1;

    class Animator* _animator = nullptr;
};
