#pragma once
#include "Utility/SingletonHelper.h"
#include "WeaponModelData.h"

class AnimationComponent;
class ParticleComponent;
class WeaponModelManager;

struct WeaponPool
{
    File::Guid                              PrefabGuid;
    std::vector<std::weak_ptr<GameObject>>  GameObjectPool;
    std::vector<AnimationComponent*>        AnimationPool;
    std::vector<ParticleComponent*>         ParticlePool;
    std::set<size_t>                        AvailableIndices;

    std::unordered_set<std::string>         WeaponAnimationKeySet;
    std::vector<std::string>                NormalAnimationKeyList;
    std::vector<std::string>                SpecialAnimationKeyList;
 };

class WeaponModelManager : public Component
{
    USING_PROPERTY(WeaponModelManager)

    using AnimationPool = std::vector<class AnimationComponent*>;
    using ParticlePool  = std::vector<class ParticleComponent*>;

public:
    WeaponModelManager();
    ~WeaponModelManager();

    const File::Guid& GetWeaponPrefabGuid(WeaponType type) const;
    /// <summary>무기 모델 오프셋 위치를 얻어옵니다.</summary>
    Vector3 GetWeaponOffsetPosition(WeaponType type) const;
    /// <summary>무기 모델 오프셋 회전(오일러)을 얻어옵니다.</summary>
    Vector3 GetWeaponOffsetRotation(WeaponType type) const;
    /// <summary>무기 모델 오프셋 거리를 얻어옵니다.</summary>
    float GetWeaponOffsetDistance(WeaponType type) const;

    WeaponModelData RequestAvailableWeapon(WeaponType type);
    bool            ReturnWeaponModel(WeaponModelData data);

    bool  HasWeaponAnimation(WeaponType type, const std::string& animKey);
    const std::string* GetRandomWeaponAnimationKeyToNormalAttack(WeaponType type);
    const std::string* GetRandomWeaponAnimationKeyToSpecialAttack(WeaponType type);


private:
    void Awake() override;
    void Start() override;

    void ImGuiDrawPropertysEvent() override;

    void UpdateOffsetPosition();
    void UpdateOffsetRotation();
    void UpdateOffsetDistance();

    void InitializeWeaponPool();
    void LoadWeaponInstances(WeaponType type, const File::Guid& prefabGuid);

private:
    SingletonComponent<WeaponModelManager>      _singletonComponent{this};

    std::unordered_map<WeaponType, WeaponPool>  _weaponPoolTable;
    std::unordered_map<WeaponType, Vector3>     _weaponOffsetPositionTable;
    std::unordered_map<WeaponType, Vector3>     _weaponOffsetRotationTable;
    std::unordered_map<WeaponType, float>       _weaponOffsetDistanceTable;

    REFLECT_FIELDS_BEGIN(Component)
    std::unordered_map<std::string, std::string> WeaponPrefabGuidTable;
    std::unordered_map<std::string, std::array<float, 3>> WeaponOffsetPositionTable;
    std::unordered_map<std::string, std::array<float, 3>> WeaponOffsetRotationTable;
    std::unordered_map<std::string, float> WeaponOffsetDistance;
    REFLECT_FIELDS_END(WeaponModelManager)

    constexpr static int WEAPON_POOLING_SIZE = 10;
};