#include "pchScripts.h"
#include "MonsterSystem.h"

#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
#include "Stats/CharacterStats.h"

#include "ExcelDataSystem/ExcelDataSystem.h"

UMREAL_COMPONENT(MonsterSystem)

using namespace Monster;

void MonsterSystem::Reset()
{
}

void MonsterSystem::Awake()
{
    _singletonComponent.TrySingleTon();
    _singletonObject.TrySingleTon(true);
}

void MonsterSystem::Start() 
{
    if (_singletonComponent.IsSingleTon())
    {
        LoadFromExcelData();
    }
}

void MonsterSystem::OnDestroy() 
{
    Clear();
}

void MonsterSystem::ImGuiDrawPropertysEvent() 
{
    if (ImGui::TreeNodeEx("Spawned Enemies", ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (const auto& [spawnPoint, weakEnemy] : _spawnedEnemyTable)
        {
            const char* spawnPointName = SpawnPointToString(spawnPoint);
            ImGui::Text("Spawn Point %s - ", spawnPointName);
            ImGui::SameLine();
            if (Enemy* enemy = weakEnemy.lock().get())
            {
                Controller& controller  = enemy->GetController();
                const DataContext& data = controller.GetDataContext();
                ImGui::Text("%d : %s", data.ID, data.Name.c_str());
            }
            else
            {
                ImGuiHelper::StyleBuilder styleBuilder;
                styleBuilder.PushStyleColor(ImGuiCol_Text, IM_COL32(255, 100, 100, 255));
                ImGui::Text("Expired");
            }
        }
        ImGui::TreePop();
    }
}

const DataContext* MonsterSystem::GetDataContextFromID(DataID id)
{
    if (_monsterDataTable.contains(id))
    {
        return &_monsterDataTable[id];
    }
    return nullptr;
}

const ActionContext* MonsterSystem::GetActionContextFromID(ActionID id)
{
    if (_actionDataTable.contains(id))
    {
        return &_actionDataTable[id];
    }
    return nullptr;
}

bool MonsterSystem::SpawnMonsterFromSpawnID(SpawnID spawnID, Difficulty difficulty)
{
    ClearSpawnedEnemies();
    bool contains = _spawnDataTable.contains(spawnID);
    assert(contains); // [assert] 해당 스폰 ID가 스폰 데이터 테이블에 존재해야합니다.
    if (contains)
    {
        FindSpawnPoints();

        size_t      succeed   = 0;
        const auto& spawnData = _spawnDataTable[spawnID];
        for (size_t i = 0; i < spawnData.SpawnParams.size(); ++i)
        {
            const auto& spawnParam = spawnData.SpawnParams[i];
            bool isValid = _monsterDataTable.contains(spawnParam.MonsterID);
            if (isValid)
            {
                SpawnPoint spawnPointType = static_cast<SpawnPoint>(i);
                if (SpawnMonsterFromSpawnID(spawnID, spawnPointType, difficulty))
                {
                    ++succeed;
                }
            }
        }
        return succeed == spawnData.SpawnParams.size();
    }
    return false;
}

bool MonsterSystem::SpawnMonsterFromSpawnID(SpawnID spawnID, SpawnPoint spawnPointType, Difficulty difficulty)
{
    assert(spawnPointType != SpawnPoint::Invalid); // [assert] 스폰 포인트 타입이 Invalid가 아니어야합니다.
    if (spawnPointType == SpawnPoint::Invalid)
    {
        return false;
    }

    if (_spawnDataTable.contains(spawnID))
    {
        auto&  spawnData  = _spawnDataTable[spawnID];
        size_t diffIndex  = static_cast<size_t>(difficulty);
        size_t spawnIndex = static_cast<size_t>(spawnPointType);

        // [assert] 난이도 인덱스가 최대 난이도 수를 넘지 않아야합니다.
        assert(diffIndex < MAX_DIFF_COUNT);

        GameObject* spawnPoint = _spawnPointTable[spawnPointType].lock().get();
        assert(spawnPoint); // [assert] 해당 인덱스의 적 스폰 포인트가 존재하지 않습니다.
        if (spawnPoint)
        {
            SpawnParam& spawnParam = spawnData.SpawnParams[spawnIndex];
            LevelID     levelID    = spawnData.LevelID[diffIndex];
            DataID      monsterID  = spawnParam.MonsterID;

            std::weak_ptr<Enemy> weakClone = SpawnMonster(levelID, monsterID);
            // 스폰에 성공했을 시 
            if (Enemy* clone = weakClone.lock().get())
            {
                // 해당 함수를 성공했다는 것은 데이터 테이블에 해당 데이터가 존재한다는 뜻이므로 예외 생략
                const StatContext& statData    = _statDataTable[levelID][monsterID];
                const DataContext& dataContext = _monsterDataTable[monsterID];

                // 컴포넌트 유효성 검사
                bool validClone = clone->FindComponent();
                assert(validClone); // [assert] 스폰된 몬스터에 필요한 컴포넌트가 존재하지 않습니다.

                if (validClone)
                {
                    // 위치 및 회전 설정
                    SetMonsterTransformToSpawnPoint(clone, spawnPointType);

                    // 스탯 설정
                    SetMonsterStateFromStatContext(clone, &statData);

                    // 컨트롤러 빌드
                    Controller& controller = clone->GetController();
                    controller.Build(weakClone, &dataContext, &statData, &spawnData);

                    // 테이블 등록
                    _spawnedEnemyTable[spawnPointType] = weakClone;
                    _spawnedEnemiesIDTable[monsterID].push_back(weakClone);

                    return true;
                }
            }
        }
    }
    return false;
}

void MonsterSystem::ClearSpawnedEnemies() 
{
    _spawnedEnemyTable.clear();
    _spawnedEnemiesIDTable.clear();
}

std::weak_ptr<Enemy> MonsterSystem::SpawnMonster(LevelID levelID, DataID monsterID)
{
    bool statTableContains = _statDataTable.contains(levelID);
    assert(statTableContains); // [assert] 해당 레벨에 몬스터 스탯 데이터 테이블이 존재해야합니다.
    if (statTableContains)
    {
        auto& statDataTable = _statDataTable[levelID];
        bool  statContains  = statDataTable.contains(monsterID);
        if (statContains)
        {
            const StatContext&   statData  = statDataTable[monsterID];
            std::weak_ptr<Enemy> weakClone = SpawnMonster(monsterID);
            if (auto clone = weakClone.lock().get())
            {
                
                return weakClone;
            }
        }
    }
    return std::weak_ptr<Enemy>();
}

std::weak_ptr<Enemy> MonsterSystem::SpawnMonster(DataID dataID)
{
    bool dataContains = _monsterDataTable.contains(dataID);
    assert(dataContains); // [assert] 해당 몬스터 데이터가 존재해야합니다.
    if (dataContains)
    {
        const DataContext& dataContext = _monsterDataTable[dataID];
        const File::Guid&  prefabGuid  = UmFileSystem.GetGuidFromAssetID(dataContext.PrefabID);
        assert(prefabGuid != File::NULL_GUID); // [assert] 콘텍스트가 가진 프리팹 ID가 유효한 Guid를 반환해야합니다.

        auto sharedPtrObject = UmGameObjectFactory.DeserializeToGuid(prefabGuid);
        assert(sharedPtrObject); // [assert] 프리팹 Guid를 통해 프리팹을 만들지 못했습니다.

        GameObject* clone = sharedPtrObject.get();
        if (clone)
        {
            Enemy* enemy = clone->GetComponent<Enemy>();
            auto   sharedEnemy = std::static_pointer_cast<Enemy>(enemy->GetWeakPtr().lock());
            assert(sharedEnemy); // [assert] 프리팹에 Enemy 컴포넌트가 존재해야합니다.
            return sharedEnemy;
        }
    }
    return std::weak_ptr<Enemy>();
}

void MonsterSystem::SetMonsterTransformToSpawnPoint(Enemy* dest, SpawnPoint spawnPointType)
{
    if (dest)
    {
        assert(spawnPointType != SpawnPoint::Invalid); // [assert] 스폰 포인트 타입이 Invalid가 아니어야합니다.
        if (spawnPointType != SpawnPoint::Invalid)
        {
            bool contains = _spawnPointTable.contains(spawnPointType);
            assert(contains); // [assert] 해당 스폰 포인트 타입이 스폰 포인트 테이블에 존재해야합니다.

            if (auto spawnPoint = _spawnPointTable[spawnPointType].lock())
            {
                // 위치 및 회전 설정
                dest->SetPositionFromSpawnPoint(spawnPointType);

                // 부모 설정
                if (GameObject* spawnGroup = _spawnGroup.lock().get())
                {
                    dest->transform->SetParent(spawnGroup->transform, false);
                }
            }
        }
       
    }
}

void MonsterSystem::SetMonsterStateFromStatContext(Enemy* dest, const StatContext* pStatContext)
{
    if (dest)
    {
        if (auto stats = dest->GetCharacterStats())
        {
            stats->MaxHP          = pStatContext->Health;
            stats->CurrentHP      = pStatContext->Health;
            stats->StunResistance = pStatContext->StunResist;
        }
    }
}

void MonsterSystem::Clear()
{
    ClearSpawnedEnemies();
    ClearDataTables();
}

void MonsterSystem::ClearDataTables()
{
    _monsterDataTable.clear();
    _actionDataTable.clear();
    _statDataTable.clear();
    _spawnDataTable.clear();
}

const std::vector<std::weak_ptr<Enemy>>* MonsterSystem::GetSpawnedEnemiesFromID(DataID dataID) const
{
    auto it = _spawnedEnemiesIDTable.find(dataID);
    if (it != _spawnedEnemiesIDTable.end())
    {
        return &it->second;
    }
    return nullptr;
}

std::weak_ptr<Enemy> MonsterSystem::GetSpawnedEnemyFromSpawnPoint(SpawnPoint spawnPointType) const
{
    auto it = _spawnedEnemyTable.find(spawnPointType);
    if (it != _spawnedEnemyTable.end())
    {
        return it->second;
    }
    return std::weak_ptr<Enemy>();
}

std::weak_ptr<GameObject> MonsterSystem::GetSpawnPointObject(SpawnPoint spawnPointType) const
{
    auto it = _spawnPointTable.find(spawnPointType);
    if (it != _spawnPointTable.end())
    {
        return it->second;
    }
    return std::weak_ptr<GameObject>();
}

std::unordered_map<SpawnPoint, std::weak_ptr<Enemy>> MonsterSystem::GetSpawnedEnemiesTable() const
{
    return _spawnedEnemyTable;
}

void MonsterSystem::FindSpawnPoints() 
{
    _spawnGroup = GameObject::Find("TurnActors Group");
    assert(_spawnGroup.expired() == false); // [assert] 턴 액터 그룹이 유효해야합니다.

    for (size_t i = 0; i < MAX_ENEMY_COUNT; ++i)
    {
        SpawnPoint index = static_cast<SpawnPoint>(i);
        if (_spawnPointTable[index].expired())
        {
            const std::weak_ptr<GameObject> weakGameObject = GameObject::FindWithTag(SPAWN_POINT_TAGS[i]);
            assert(weakGameObject.expired() == false); // [assert] 해당 태그의 스폰 포인트가 유효해야합니다.
            _spawnPointTable[index] = weakGameObject;
        }
    }
}

void MonsterSystem::LoadFromExcelData()
{
    if (ExcelDataSystem* dataSystem = SingletonComponent<ExcelDataSystem>::GetInstance())
    {
        LoadDataContextFromExcelData(dataSystem);
        LoadActionContextFromExcelData(dataSystem);
        LoadStatContextFromExcelData(dataSystem);
        LoadSpawnContextFromExcelData(dataSystem);
    }
}

namespace ExcelDataKey   = ExcelData::Key::Data;
namespace ExcelActionKey = ExcelData::Key::Action;
namespace ExcelStatKey   = ExcelData::Key::Stat;
namespace ExcelSpawnKey  = ExcelData::Key::Spawn;

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
                DataContext      context;
                std::string_view data;
                data = dataBase->FindData(rowIndex, ExcelDataKey::ID);
                if (data != ExcelDataBase::FIND_STR_FAIL)
                {
                    context.ID = StringToInt(data);
                }
                data = dataBase->FindData(rowIndex, ExcelDataKey::NAME);
                if (data != ExcelDataBase::FIND_STR_FAIL)
                {
                    context.Name = std::string(data);
                }
                data = dataBase->FindData(rowIndex, ExcelDataKey::MODEL_ID);
                if (data != ExcelDataBase::FIND_STR_FAIL)
                {
                    context.PrefabID = StringToInt(data);
                }

                for (size_t i = 0; i < MAX_FSM_COUNT; ++i)
                {
                    data = dataBase->FindData(rowIndex, ExcelDataKey::FSM[i]);
                    if (data != ExcelDataBase::FIND_STR_FAIL)
                    {
                        int fsmID = StringToInt(data);
                        if (0 != fsmID)
                        {
                            context.FsmIDs.push_back(fsmID);
                        }
                    }
                }

                for (size_t i = 0; i < MAX_SKILL_COUNT; ++i)
                {
                    data = dataBase->FindData(rowIndex, ExcelDataKey::SKILL[i]);
                    if (data != ExcelDataBase::FIND_STR_FAIL)
                    {
                        int skillID = StringToInt(data);
                        if (0 != skillID)
                        {
                            context.ActionIDs.push_back(skillID);
                        }
                    }
                }

                _monsterDataTable[context.ID] = std::move(context);
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
                ActionContext    context;
                std::string_view data;
                data = dataBase->FindData(rowIndex, ExcelActionKey::ID);
                if (data != ExcelDataBase::FIND_STR_FAIL)
                {
                    context.ID = StringToInt(data);
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
                    context.AttackCount = StringToInt(data);
                }
                data = dataBase->FindData(rowIndex, ExcelActionKey::PARAMETER);
                if (data != ExcelDataBase::FIND_STR_FAIL)
                {
                    context.Parameter = std::string(data);
                }
                _actionDataTable[context.ID] = std::move(context);
            }
        }
    }
}

void MonsterSystem::LoadStatContextFromExcelData(ExcelDataSystem* dataSystem)
{
    if (dataSystem)
    {
       
        for (auto& diffStr : ExcelStatKey::DIFFICULTY_LIST)
        {
            std::u8string sheetName = diffStr;
            sheetName += ExcelStatKey::SHEET_NAME;
            std::unique_ptr<ExcelDataBase> dataBase = dataSystem->FindExcelDataBase(sheetName.c_str());
            assert(dataBase); // [assert] 엑셀 데이터 시스템에 해당 시트가 존재해야합니다.
            if (dataBase)
            {
                const size_t rowCount = dataBase->RowCount();
                for (size_t rowIndex = 0; rowIndex < rowCount; ++rowIndex)
                {
                    StatContext      context;
                    std::string_view data;
                    data = dataBase->FindData(rowIndex, ExcelStatKey::LEVEL_ID);
                    if (data != ExcelDataBase::FIND_STR_FAIL)
                    {
                        context.LevelID = StringToInt(data);
                    }
                    data = dataBase->FindData(rowIndex, ExcelStatKey::MONSTER_ID);
                    if (data != ExcelDataBase::FIND_STR_FAIL)
                    {
                        context.MonsterID = StringToInt(data);
                    }
                    data = dataBase->FindData(rowIndex, ExcelStatKey::HEALTH);
                    if (data != ExcelDataBase::FIND_STR_FAIL)
                    {
                        context.Health = StringToInt(data);
                    }
                    data = dataBase->FindData(rowIndex, ExcelStatKey::STUN_RESIST);
                    if (data != ExcelDataBase::FIND_STR_FAIL)
                    {
                        context.StunResist = StringToInt(data);
                    }
                    data = dataBase->FindData(rowIndex, ExcelStatKey::PARAM);
                    if (data != ExcelDataBase::FIND_STR_FAIL)
                    {
                        const auto params = ParseParam(std::string(data));
                        for (const auto& param : params)
                        {
                            context.StatParams.push_back({param});
                        }
                    }
                    for (size_t i = 0; i < MAX_SKILL_COUNT; ++i)
                    {
                        data = dataBase->FindData(rowIndex, ExcelStatKey::ACTION_PARAM[i]);
                        if (data != ExcelDataBase::FIND_STR_FAIL)
                        {
                            const auto params = ParseParam(std::string(data));
                            for (const auto& param : params)
                            {
                                context.ActionParams[i].push_back({param});
                            }
                        }
                        data = dataBase->FindData(rowIndex, ExcelStatKey::TOKEN_PARAM[i]);
                        if (data != ExcelDataBase::FIND_STR_FAIL)
                        {
                            context.TokenParams[i] = ParseTokenParam(std::string(data));
                        }
                    }
                    _statDataTable[context.LevelID][context.MonsterID] = std::move(context);
                }
            }
        }
    }
}

void MonsterSystem::LoadSpawnContextFromExcelData(ExcelDataSystem* dataSystem)
{
    if (dataSystem)
    {
        std::unique_ptr<ExcelDataBase> dataBase = dataSystem->FindExcelDataBase(ExcelSpawnKey::SHEET_NAME);
        assert(dataBase); // [assert] 엑셀 데이터 시스템에 해당 시트가 존재해야합니다.
        if (dataBase)
        {
            const size_t rowCount = dataBase->RowCount();
            for (size_t rowIndex = 0; rowIndex < rowCount; ++rowIndex)
            {
                SpawnContext context;
                std::string_view data;
                data = dataBase->FindData(rowIndex, ExcelSpawnKey::ID);
                if (data != ExcelDataBase::FIND_STR_FAIL)
                {
                    context.SpawnID = StringToInt(data);
                }
                for (size_t i = 0; i < MAX_DIFF_COUNT; ++i)
                {
                    data = dataBase->FindData(rowIndex, ExcelSpawnKey::LEVEL_ID[i]);
                    if (data != ExcelDataBase::FIND_STR_FAIL)
                    {
                        context.LevelID[i] = StringToInt(data);
                    }
                }
                for (size_t i = 0; i < MAX_ENEMY_COUNT; ++i)
                {
                    SpawnParam& spawnParam = context.SpawnParams[i];
                    data = dataBase->FindData(rowIndex, ExcelSpawnKey::TILE[i]);
                    if (data != ExcelDataBase::FIND_STR_FAIL && false == data.empty())
                    {
                        spawnParam.MonsterID = StringToInt(data);
                    }
                    data = dataBase->FindData(rowIndex, ExcelSpawnKey::BUFF[i]);
                    if (data != ExcelDataBase::FIND_STR_FAIL && false == data.empty())
                    {
                        std::vector<TokenParam> params = ParseTokenParam(std::string(data));
                        for (auto& param : params)
                        {
                            spawnParam.InitialTokens.push_back(param);
                        }
                    }
                }

                _spawnDataTable[context.SpawnID] = std::move(context);
            }
        }
    }
}
