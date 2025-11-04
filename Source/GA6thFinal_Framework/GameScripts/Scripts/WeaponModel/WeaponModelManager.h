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
    Vector3           GetWeaponOffset(WeaponType type) const;
    Quaternion        GetWeaponOriginPivotRotation(WeaponType type) const;

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

    void InitializeWeaponPool();
    void LoadWeaponInstances(WeaponType type, const File::Guid& prefabGuid);

private:
    SingletonComponent<WeaponModelManager>      _singletonComponent{this};

    std::unordered_map<WeaponType, WeaponPool>  _weaponPoolTable;
    std::unordered_map<WeaponType, Vector3>     _availableWeaponOffsetsTable;
    std::unordered_map<WeaponType, Quaternion>  _OriginPivotRotation;

    REFLECT_FIELDS_BEGIN(Component)
    std::unordered_map<std::string, std::string> WeaponPrefabGuidTable;
    std::unordered_map<std::string, std::array<float, 3>> WeaponPrefabOffsetTable;
    REFLECT_FIELDS_END(WeaponModelManager)

    constexpr static int WEAPON_POOLING_SIZE = 10;
};