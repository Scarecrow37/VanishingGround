#include "pchScripts.h"
#include "MonsterSystem.h"

#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
#include "Stats/CharacterStats.h"

#include "ExcelDataSystem/ExcelDataSystem.h"

UMREAL_COMPONENT(MonsterSystem)

void MonsterSystem::Reset()
{
}

void MonsterSystem::Awake()
{
    if (_singletonComponent.TrySingleTon())
    {
        FindSpawnPoints();
        LoadFromExcelData();
    }
}

void MonsterSystem::OnDestroy() 
{
    Clear();
}

const Monster::DataContext* MonsterSystem::GetDataContextFromID(Monster::DataID id)
{
    if (_dataContextTable.contains(id))
    {
        return &_dataContextTable[id];
    }
    return nullptr;
}

const Monster::ActionContext* MonsterSystem::GetActionContextFromID(Monster::ActionID id)
{
    if (_actionContextTable.contains(id))
    {
        return &_actionContextTable[id];
    }
    return nullptr;
}

bool MonsterSystem::SpawnMonsterFromStageID(int stageID)
{
    if (auto pStageEnemiesData = GetStatContextFromStageID(stageID))
    {
        const auto& stageEnemiesData = *pStageEnemiesData;
        // [assert] 스테이지 몬스터 데이터의 크기는 최대 적 수와 같아야합니다.
        assert(stageEnemiesData.size() == Monster::MAX_ENEMY_COUNT);

        for (size_t i = 0; i < Monster::MAX_ENEMY_COUNT; ++i)
        {
            if (stageEnemiesData.size() > i)
            {
                const Monster::StatContext& enemyStageData = stageEnemiesData[i];
                SpawnMonsterFromDataContext(&enemyStageData, i);
            }
        }
    }
    return false;
}

std::weak_ptr<Enemy> MonsterSystem::SpawnMonsterFromDataContext(const Monster::StatContext* pStatContext, size_t index)
{
    if (pStatContext)
    {
        if (const Monster::DataContext* pDataContext = GetDataContextFromID(pStatContext->MonsterID))
        {
            std::weak_ptr<Enemy> weakClone = SpawnMonsterFromDataContext(pDataContext, index);
            if (auto clone = weakClone.lock())
            {
                if (auto stats = clone->GetCharacterStats())
                {
                    stats->MaxHP          = pStatContext->Health;
                    stats->CurrentHP      = pStatContext->Health;
                    stats->StunResistance = pStatContext->StunResist;
                }
                Monster::Controller& controller = clone->GetController();
                controller.Build(weakClone, pDataContext, pStatContext);
            }
            return weakClone;
        }
    }
    return std::weak_ptr<Enemy>();
}

std::weak_ptr<Enemy> MonsterSystem::SpawnMonsterFromDataContext(const Monster::DataContext* context, size_t index)
{
    if (context)
    {
        if (index < Monster::MAX_ENEMY_COUNT)
        {
            // [assert] 해당 인덱스의 적 스폰 포인트가 존재하지 않습니다.
            assert(_enemySpawnPoints[index].expired() == false);
            GameObject* spawnPoint = _enemySpawnPoints[index].lock().get();
            assert(spawnPoint);
            if (spawnPoint)
            {
                const Transform&  transform     = spawnPoint->transform;
                const Vector3     worldPosition = transform.WorldPosition;
                const Vector3     eulerAngles   = transform.EulerAngle;
                const File::Guid& prefabGuid    = UmFileSystem.GetGuidFromAssetID(context->PrefabID);
                assert(prefabGuid != File::NULL_GUID); // [assert] 콘텍스트가 가진 프리팹 ID가 유효한 Guid를 반환해야합니다.

                auto sharedPtrObject = UmGameObjectFactory.DeserializeToGuid(prefabGuid);
                assert(sharedPtrObject); // [assert] 프리팹 Guid를 통해 프리팹을 만들지 못했습니다.

                GameObject* clone = sharedPtrObject.get();
                if (clone)
                {
                    Enemy* enemy = clone->GetComponent<Enemy>();
                    assert(enemy); // [assert] 프리팹에 Enemy 컴포넌트가 존재해야합니다.
                    if (enemy)
                    {
                        clone->transform->WorldPosition = worldPosition;
                        clone->transform->EulerAngle    = eulerAngles;

                        auto sharedEnemy       = std::static_pointer_cast<Enemy>(enemy->GetWeakPtr().lock());
                        _spawnedEnemies[index] = sharedEnemy;
                        _spawnedEnemiesIDTable[context->ID].push_back(sharedEnemy);

                        return sharedEnemy;
                    }
                }
            }
        }
    }
    return std::weak_ptr<Enemy>();
}

void MonsterSystem::Clear()
{
    ClearSpawnedEnemies();
    for (size_t i = 0; i < Monster::MAX_ENEMY_COUNT; ++i)
    {
        _spawnedEnemies[i].reset();
    }
    _dataContextTable.clear();
    _actionContextTable.clear();
    _StatContextTable.clear();
}

void MonsterSystem::ClearSpawnedEnemies()
{
    for (size_t i = 0; i < Monster::MAX_ENEMY_COUNT; ++i)
    {
        if (auto enemy = _spawnedEnemies[i].lock())
        {
            _spawnedEnemies[i].reset();
            GameObject& object = enemy->gameObject;
            GameObject::Destroy(object);
        }
    }
    _spawnedEnemiesIDTable.clear();
}

std::weak_ptr<Enemy> MonsterSystem::GetSpawnedEnemyByIndex(size_t index)
{
    if (index < Monster::MAX_ENEMY_COUNT)
    {
        return _spawnedEnemies[index];
    }
    return std::weak_ptr<Enemy>();
}

std::vector<std::weak_ptr<Enemy>> MonsterSystem::GetSpawnedEnemyByID(Monster::DataID id)
{
    if (_spawnedEnemiesIDTable.contains(id))
    {
        return _spawnedEnemiesIDTable[id];
    }
    return std::vector<std::weak_ptr<Enemy>>();
}

void MonsterSystem::FindSpawnPoints() 
{
    for (size_t i = 0; i < Monster::MAX_ENEMY_COUNT; ++i)
    {
        const std::weak_ptr<GameObject> weakGameObject = GameObject::FindWithTag(Monster::SPAWN_POINT_TAGS[i]);
        assert(weakGameObject.expired() == false); // [assert] 해당 태그의 스폰 포인트가 유효해야합니다.
        _enemySpawnPoints[i] = weakGameObject;
    }
}

void MonsterSystem::LoadFromExcelData()
{
    if (ExcelDataSystem* dataSystem = SingletonComponent<ExcelDataSystem>::GetInstance())
    {
        LoadDataContextFromExcelData(dataSystem);
        LoadActionContextFromExcelData(dataSystem);
        LoadStatContextFromExcelData(dataSystem);
    }
}

namespace ExcelDataKey   = Monster::ExcelData::Key::Data;
namespace ExcelActionKey = Monster::ExcelData::Key::Action;
namespace ExcelStageKey  = Monster::ExcelData::Key::Stage;

void MonsterSystem::LoadDataContextFromExcelData(ExcelDataSystem* dataSystem)
{
    if (dataSystem)
    {
        std::unique_ptr<ExcelDataBase> dataBase = dataSystem->FindExcelDataBase(ExcelDataKey::SHEET_NAME);
        assert(dataBase); // [assert] 엑셀 데이터 시스템에 해당 시트가 존재해야합니다.
        if (dataBase)
        {
            const size_t rowCount = dataBase->RowCount();
            for (size_t rowIndex = 0; rowIndex < rowCount; ++rowIndex)
            {
                if (rowIndex != ExcelDataBase::FIND_INDEX_FAIL)
                {
                    Monster::DataContext context;
                    std::string_view     data;
                    data = dataBase->FindData(rowIndex, ExcelDataKey::ID);
                    if (data != ExcelDataBase::FIND_STR_FAIL)
                    {
                        context.ID = std::stoi(data.data());
                    }
                    data = dataBase->FindData(rowIndex, ExcelDataKey::NAME);
                    if (data != ExcelDataBase::FIND_STR_FAIL)
                    {
                        context.Name = std::string(data);
                    }
                    data = dataBase->FindData(rowIndex, ExcelDataKey::MODEL_ID);
                    if (data != ExcelDataBase::FIND_STR_FAIL)
                    {
                        context.PrefabID = std::stoi(data.data());
                    }

                    for (size_t i = 0; i < Monster::MAX_FSM_COUNT; ++i)
                    {
                        data = dataBase->FindData(rowIndex, ExcelDataKey::FSM[i]);
                        if (data != ExcelDataBase::FIND_STR_FAIL)
                        {
                            context.FsmIDs.push_back(std::stoi(data.data()));
                        }
                    }

                    for (size_t i = 0; i < Monster::MAX_SKILL_COUNT; ++i)
                    {
                        data = dataBase->FindData(rowIndex, ExcelDataKey::SKILL[i]);
                        if (data != ExcelDataBase::FIND_STR_FAIL)
                        {
                            context.ActionIDs.push_back(std::stoi(data.data()));
                        }
                    }

                    _dataContextTable[context.ID] = std::move(context);
                }
            }
        }
    }
}

void MonsterSystem::LoadActionContextFromExcelData(ExcelDataSystem* dataSystem)
{
    if (dataSystem)
    {
        std::unique_ptr<ExcelDataBase> dataBase = dataSystem->FindExcelDataBase(ExcelActionKey::SHEET_NAME);
        assert(dataBase); // [assert] 엑셀 데이터 시스템에 해당 시트가 존재해야합니다.
        if (dataBase)
        {
            const size_t rowCount = dataBase->RowCount();
            for (size_t rowIndex = 0; rowIndex < rowCount; ++rowIndex)
            {
                if (rowIndex != ExcelDataBase::FIND_INDEX_FAIL)
                {
                    Monster::ActionContext context;
                    std::string_view       data;
                    data = dataBase->FindData(rowIndex, ExcelActionKey::ID);
                    if (data != ExcelDataBase::FIND_STR_FAIL)
                    {
                        context.ID = std::stoi(data.data());
                    }
                    data = dataBase->FindData(rowIndex, ExcelActionKey::NAME);
                    if (data != ExcelDataBase::FIND_STR_FAIL)
                    {
                        context.Name = std::string(data);
                    }
                    data = dataBase->FindData(rowIndex, ExcelActionKey::TYPE);
                    if (data != ExcelDataBase::FIND_STR_FAIL)
                    {
                        context.Type = std::string(data);
                    }
                    data = dataBase->FindData(rowIndex, ExcelActionKey::TARGET);
                    if (data != ExcelDataBase::FIND_STR_FAIL)
                    {
                        context.Target = std::string(data);
                    }
                    data = dataBase->FindData(rowIndex, ExcelActionKey::ATTACK_COUNT);
                    if (data != ExcelDataBase::FIND_STR_FAIL)
                    {
                        context.AttackCount = std::stoi(data.data());
                    }
                    data = dataBase->FindData(rowIndex, ExcelActionKey::PARAMETER);
                    if (data != ExcelDataBase::FIND_STR_FAIL)
                    {
                        context.Parameter = std::string(data);
                    }
                    _actionContextTable[context.ID] = std::move(context);
                }
            }
        }
    }
}

void MonsterSystem::LoadStatContextFromExcelData(ExcelDataSystem* dataSystem)
{
    if (dataSystem)
    {
        bool isNormalDiff = true ; // TODO: 나중에 난이도에 따라 다른 시트를 불러오도록 변경
        std::u8string sheetName = isNormalDiff? u8"(일반)" : u8"(어려움)"; 
        sheetName += ExcelStageKey::SHEET_NAME;
        
        std::unique_ptr<ExcelDataBase> dataBase = dataSystem->FindExcelDataBase(sheetName.c_str());
        assert(dataBase); // [assert] 엑셀 데이터 시스템에 해당 시트가 존재해야합니다.
        if (dataBase)
        {
            const size_t rowCount = dataBase->RowCount();
            for (size_t rowIndex = 0; rowIndex < rowCount; ++rowIndex)
            {
                if (rowIndex != ExcelDataBase::FIND_INDEX_FAIL)
                {
                    Monster::StatContext context;
                    std::string_view      data;
                    data = dataBase->FindData(rowIndex, ExcelStageKey::LEVEL_ID);
                    if (data != ExcelDataBase::FIND_STR_FAIL)
                    {
                        context.LevelID = std::stoi(data.data());
                    }
                    data = dataBase->FindData(rowIndex, ExcelStageKey::MONSTER_ID);
                    if (data != ExcelDataBase::FIND_STR_FAIL)
                    {
                        context.MonsterID = std::stoi(data.data());
                    }
                    data = dataBase->FindData(rowIndex, ExcelStageKey::HEALTH);
                    if (data != ExcelDataBase::FIND_STR_FAIL)
                    {
                        context.Health = std::stoi(data.data());
                    }
                    data = dataBase->FindData(rowIndex, ExcelStageKey::STUN_RESIST);
                    if (data != ExcelDataBase::FIND_STR_FAIL)
                    {
                        context.StunResist = std::stoi(data.data());
                    }

                    for (size_t i = 0; i < Monster::MAX_SKILL_COUNT; ++i)
                    {
                        data = dataBase->FindData(rowIndex, ExcelStageKey::ACTION_PARAM[i]);
                        if (data != ExcelDataBase::FIND_STR_FAIL)
                        {
                            context.ActionParams[i] = Monster::ParseActionParam(std::string(data));
                        }
                        data = dataBase->FindData(rowIndex, ExcelStageKey::TOKEN_PARAM[i]);
                        if (data != ExcelDataBase::FIND_STR_FAIL)
                        {
                            context.TokenParams[i] = Monster::ParseTokenParam(std::string(data));
                        }
                    }

                    // 벡터 초기화
                    if (false == _StatContextTable.contains(context.LevelID))
                    {
                        _StatContextTable[context.LevelID].reserve(Monster::MAX_ENEMY_COUNT);
                    }
                    _StatContextTable[context.LevelID].push_back(std::move(context));
                }
            }
        }
    }
}

const std::vector<Monster::StatContext>* MonsterSystem::GetStatContextFromStageID(int stageID)
{
    if (_StatContextTable.contains(stageID))
    {
        return &_StatContextTable[stageID];
    }
    return nullptr;
}