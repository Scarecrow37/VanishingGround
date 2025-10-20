#pragma once
#include <Utility/SingletonHelper.h>

#include "Monster/Common/MonsterCommon.h"
#include "Monster/Controller/MonsterController.h"

#include "Monster/Context/MonsterDataContext.h"
#include "Monster/Context/MonsterActionContext.h"
#include "Monster/Context/MonsterStatContext.h"
#include "Monster/Context/MonsterSpawnContext.h"

#include "DifficultyManager/DifficultyEnum.h"

class ExcelDataSystem;
class ExcelDataBase;
class Enemy;

class MonsterSystem : public Component
{
    USING_PROPERTY(MonsterSystem)

    using SpawnedEnemyTable = std::array<std::weak_ptr<Enemy>, Monster::MAX_ENEMY_COUNT>;
    using StatDataTable = std::unordered_map<Monster::LevelID, std::unordered_map<Monster::DataID, Monster::StatContext>>;
    using SpawnDataTable = std::unordered_map<Monster::SpawnID, Monster::SpawnContext>;
    using ActionDataTable = std::unordered_map<Monster::ActionID, Monster::ActionContext>;
    using MonsterDataTable = std::unordered_map<Monster::DataID, Monster::DataContext>;

public:
    MonsterSystem() = default;
    ~MonsterSystem() = default;

private:
    void Reset() override;
    void Awake() override;
    void OnDestroy() override;

public:
    /// <summary>
    /// 주어진 몬스터 데이터 ID에 해당하는 Monster::DataContext를 반환합니다.
    /// </summary>
    /// <param name="id">검색할 몬스터 데이터의 ID입니다.</param>
    /// <returns>ID에 해당하는 Monster::DataContext 객체에 대한 포인터를 반환합니다. 해당 ID가 없으면 nullptr을 반환할 수 있습니다.</returns>
    const Monster::DataContext* GetDataContextFromID(Monster::DataID id);

    /// <summary>
    /// 주어진 액션 ID에 해당하는 Monster::ActionContext 포인터를 반환합니다.
    /// </summary>
    /// <param name="id">검색할 Monster::ActionID 값입니다.</param>
    /// <returns>해당 ID에 연결된 Monster::ActionContext 객체의 포인터를 반환합니다. 해당 ID가 없으면 nullptr을 반환할 수 있습니다.</returns>
    const Monster::ActionContext* GetActionContextFromID(Monster::ActionID id);

    bool SpawnMonsterFromSpawnID(Monster::SpawnID spawnID, Difficulty difficulty = Difficulty::NORMAL);
    bool SpawnMonsterFromSpawnID(Monster::SpawnID spawnID, Monster::SpawnPoint spawnPoint, Difficulty difficulty = Difficulty::NORMAL);

    const std::vector<std::weak_ptr<Enemy>>* GetSpawnedEnemiesFromID(Monster::DataID dataID) const;
    std::weak_ptr<Enemy>                     GetSpawnedEnemyFromSpawnPoint(Monster::SpawnPoint spawnPoint) const;
    std::weak_ptr<GameObject>                GetSpawnPointObject(Monster::SpawnPoint spawnPointType) const;
    std::unordered_map<Monster::SpawnPoint, std::weak_ptr<Enemy>> GetSpawnedEnemiesTable() const;

private:
    void                 Clear();
    void                 FindSpawnPoints();
    void                 LoadFromExcelData();
    std::weak_ptr<Enemy> SpawnMonster(Monster::LevelID levelID, Monster::DataID monsterID);
    std::weak_ptr<Enemy> SpawnMonster(Monster::DataID dataID);
    void                 SetMonsterTransformToSpawnPoint(Enemy* dest, Monster::SpawnPoint spawnPoint);
    void                 SetMonsterStateFromStatContext(Enemy* dest, const Monster::StatContext* pStatContext);

    void                 LoadDataContextFromExcelData(ExcelDataSystem* dataSystem);
    void                 LoadActionContextFromExcelData(ExcelDataSystem* dataSystem);
    void                 LoadStatContextFromExcelData(ExcelDataSystem* dataSystem);
    void                 LoadSpawnContextFromExcelData(ExcelDataSystem* dataSystem);

private:
    SingletonComponent<MonsterSystem> _singletonComponent = {this};

    std::weak_ptr<GameObject> _spawnGroup;
    std::unordered_map<Monster::SpawnPoint, std::weak_ptr<GameObject>>      _spawnPointTable;
    std::unordered_map<Monster::SpawnPoint, std::weak_ptr<Enemy>>           _spawnedEnemyTable;
    std::unordered_map<Monster::DataID, std::vector<std::weak_ptr<Enemy>>>  _spawnedEnemiesIDTable;

    MonsterDataTable    _monsterDataTable;
    ActionDataTable     _actionDataTable;
    StatDataTable       _statDataTable;
    SpawnDataTable      _spawnDataTable;

    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(MonsterSystem)
};
