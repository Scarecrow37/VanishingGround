#pragma once
#include <Utility/SingletonHelper.h>

#include "Monster/Common/MonsterCommon.h"
#include "Monster/AI/MonsterAIFactory.h"

#include "Monster/Context/MonsterDataContext.h"
#include "Monster/Context/MonsterActionContext.h"
#include "Monster/Context/MonsterStageContext.h"

class ExcelDataSystem;
class ExcelDataBase;
class CharacterBase;
class Enemy;

class MonsterSystem : public Component
{
    USING_PROPERTY(MonsterSystem)

public:
    MonsterSystem() = default;
    ~MonsterSystem() = default;

private:
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

    /// <summary>
    /// 주어진 스테이지 ID를 통해 해당 스테이지 정보에 맞는 몬스터를 소환합니다.
    /// </summary>
    /// <param name="stageID">몬스터를 소환할 정보가 담겨있는 스테이지의 ID입니다.</param>
    /// <returns>몬스터 소환이 성공하면 true, 실패하면 false를 반환합니다.</returns>
    bool SpawnMonsterFromStageID(int stageID);

    /// <summary>
    /// 주어진 데이터 컨텍스트를 기반으로 한 몬스터를 스폰 포인트 인덱스에 소환합니다.
    /// </summary>
    /// <param name="context">몬스터 데이터가 포함된 DataContext 객체에 대한 포인터입니다.</param>
    /// <param name="index">스폰할 몬스터의 스폰 포인트 인덱스입니다.</param>
    /// <returns>몬스터 스폰이 성공하면 true, 실패하면 false를 반환합니다.</returns>
    bool SpawnMonsterFromDataContext(const Monster::DataContext* context, size_t index);

    /// <summary>
    /// 생성된 적을 모두 제거합니다.
    /// </summary>
    void ClearSpawnedEnemies();

    std::weak_ptr<Enemy> GetSpawnedEnemyByIndex(size_t index);
    std::vector<std::weak_ptr<Enemy>> GetSpawnedEnemyByID(Monster::DataID id);

private:
    void Clear();

    void FindSpawnPoints();
    void LoadFromExcelData();

    const std::vector<Monster::StageContext>* GetStageContextFromStageID(int stageID);

    void LoadDataContextFromExcelData(ExcelDataSystem* dataSystem);
    void LoadActionContextFromExcelData(ExcelDataSystem* dataSystem);
    void LoadStageContextFromExcelData(ExcelDataSystem* dataSystem);

private:
    SingletonComponent<MonsterSystem> _singletonComponent = {this};

    std::array<std::weak_ptr<GameObject>, Monster::MAX_ENEMY_COUNT>         _enemySpawnPoints;
    std::array<std::weak_ptr<Enemy>, Monster::MAX_ENEMY_COUNT>              _spawnedEnemies;
    std::unordered_map<Monster::DataID, std::vector<std::weak_ptr<Enemy>>>  _spawnedEnemiesIDTable;

    std::unordered_map<Monster::DataID, Monster::DataContext>       _dataContextTable;
    std::unordered_map<Monster::ActionID, Monster::ActionContext>   _actionContextTable;
    std::unordered_map<int, std::vector<Monster::StageContext>>     _stageContextTable;

    Monster::AIFactory _aiFactory;

    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(MonsterSystem)
};
