#include "pchScripts.h"
#include "MonsterSystem.h"

#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"

#include "ExcelDataSystem/ExcelDataSystem.h"

UMREAL_COMPONENT(MonsterSystem)

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
    if (auto pStageEnemiesData = GetStageContextFromStageID(stageID))
    {
        const auto& stageEnemiesData = *pStageEnemiesData;
        // [assert] 스테이지 몬스터 데이터의 크기는 최대 적 수와 같아야합니다.
        assert(stageEnemiesData.size() == Monster::MAX_ENEMY_COUNT);

        for (size_t i = 0; i < Monster::MAX_ENEMY_COUNT; ++i)
        {
            // [assert] 해당 인덱스의 적 스폰 포인트가 존재하지 않습니다.
            assert(_enemySpawnPoint[i] != nullptr);

            if (stageEnemiesData.size() > i)
            {
                const Monster::StageContext& enemyData = stageEnemiesData[i];
                if (const Monster::DataContext* dataContext = GetDataContextFromID(enemyData.MonsterID))
                {
                    Vector3 position = Vector3::Zero;
                    Vector3 eulerAngles = Vector3::Zero;
                    if (_enemySpawnPoint[i])
                    {
                        Transform& transform = _enemySpawnPoint[i]->transform;

                        position    = transform.WorldPosition;
                        eulerAngles = transform.EulerAngle;
                    }
                    SpawnMonsterFromDataContext(dataContext, position, eulerAngles);
                }
            }
        }
    }
    return false;
}

bool MonsterSystem::SpawnMonsterFromDataContext(const Monster::DataContext* context, Vector3 position, Vector3 eulerAngles)
{
    return false;
}

void MonsterSystem::Clear()
{
    _dataContextTable.clear();
    _actionContextTable.clear();
    _stageContextTable.clear();
}

void MonsterSystem::Load() 
{
    Clear();
    if (ExcelDataSystem* dataSystem = SingletonComponent<ExcelDataSystem>::GetInstance())
    {
        LoadDataContextFromExcelData(dataSystem);
        LoadActionContextFromExcelData(dataSystem);
        LoadStageContextFromExcelData(dataSystem);
    }
}

namespace ExcelDataKey   = Monster::ExcelData::Key::Data;
namespace ExcelActionKey = Monster::ExcelData::Key::Action;
namespace ExcelStageKey  = Monster::ExcelData::Key::Stage;

void MonsterSystem::LoadDataContextFromExcelData(ExcelDataSystem* dataSystem)
{
    if (dataSystem)
    {
        if (std::unique_ptr<ExcelDataBase> dataBase = dataSystem->FindExcelDataBase(ExcelDataKey::SHEET_NAME))
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
                            context.FsmIDs[i] = std::stoi(data.data());
                        }
                    }

                    for (size_t i = 0; i < Monster::MAX_SKILL_COUNT; ++i)
                    {
                        data = dataBase->FindData(rowIndex, ExcelDataKey::SKILL[i]);
                        if (data != ExcelDataBase::FIND_STR_FAIL)
                        {
                            context.ActionIDs[i] = std::stoi(data.data());
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
        if (std::unique_ptr<ExcelDataBase> dataBase = dataSystem->FindExcelDataBase(ExcelActionKey::SHEET_NAME))
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

void MonsterSystem::LoadStageContextFromExcelData(ExcelDataSystem* dataSystem)
{
    if (dataSystem)
    {
        if (std::unique_ptr<ExcelDataBase> dataBase = dataSystem->FindExcelDataBase(ExcelStageKey::SHEET_NAME))
        {
            const size_t rowCount = dataBase->RowCount();
            for (size_t rowIndex = 0; rowIndex < rowCount; ++rowIndex)
            {
                if (rowIndex != ExcelDataBase::FIND_INDEX_FAIL)
                {
                    Monster::StageContext context;
                    std::string_view      data;
                    data = dataBase->FindData(rowIndex, ExcelStageKey::STAGE_ID);
                    if (data != ExcelDataBase::FIND_STR_FAIL)
                    {
                        context.StageID = std::stoi(data.data());
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
                    _stageContextTable[context.StageID].push_back(std::move(context));
                }
            }
        }
    }
}



const std::vector<Monster::StageContext>* MonsterSystem::GetStageContextFromStageID(int stageID)
{
    if (_stageContextTable.contains(stageID))
    {
        return &_stageContextTable[stageID];
    }
    return nullptr;
}

