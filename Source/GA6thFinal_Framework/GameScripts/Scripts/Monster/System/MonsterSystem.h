#pragma once

#include "Monster/Common/MonsterCommon.h"

#include "Monster/Context/MonsterDataContext.h"
#include "Monster/Context/MonsterActionContext.h"
#include "Monster/Context/MonsterStageContext.h"

class ExcelDataSystem;
class ExcelDataBase;
class CharacterBase;
class Enemy;

class MonsterSystem : public Component
{
public:
    MonsterSystem() = default;
    ~MonsterSystem() = default;

public:
    const Monster::DataContext*     GetDataContextFromID(Monster::DataID id);
    const Monster::ActionContext*   GetActionContextFromID(Monster::ActionID id);

    /// <summary>
    /// 주어진 스테이지 ID에서 몬스터를 소환합니다.
    /// </summary>
    /// <param name="stageID">몬스터를 소환할 스테이지의 ID입니다.</param>
    /// <returns>몬스터 소환이 성공하면 true, 실패하면 false를 반환합니다.</returns>
    bool SpawnMonsterFromStageID(int stageID);

    bool SpawnMonsterFromDataContext(const Monster::DataContext* context, 
                                     Vector3 position = Vector3::Zero,
                                     Vector3 eulerAngles = Vector3::Zero);

private:
    void Clear();
    void Load();

    const std::vector<Monster::StageContext>* GetStageContextFromStageID(int stageID);

    void LoadDataContextFromExcelData(ExcelDataSystem* dataSystem);
    void LoadActionContextFromExcelData(ExcelDataSystem* dataSystem);
    void LoadStageContextFromExcelData(ExcelDataSystem* dataSystem);

private:
    std::array<const GameObject*, Monster::MAX_ENEMY_COUNT>         _enemySpawnPoint;

    std::unordered_map<Monster::DataID, Monster::DataContext>       _dataContextTable;
    std::unordered_map<Monster::ActionID, Monster::ActionContext>   _actionContextTable;
    std::unordered_map<int, std::vector<Monster::StageContext>>     _stageContextTable;
};
