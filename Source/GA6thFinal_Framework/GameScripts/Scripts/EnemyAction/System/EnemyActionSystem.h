#pragma once
#include <EnemyAction/EnemyActionBase.h>
#include <Utility/SingletonHelper.h>

class EnemyActionSystem  : public Component 
{
public:
    USING_PROPERTY(EnemyActionSystem)
    EnemyActionSystem();
    ~EnemyActionSystem() override;

private:
    void Reset() override;
    void Awake() override;
    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

public:
    EnemyAction::ActionData* GetEnemyActionDataFromID(int actionID);
    EnemyAction::ActionData* AddEnemyActionDataFromID(int actionID);

private:
    REFLECT_FIELDS_BEGIN(Component)
    std::unordered_map<int, std::string> ActionSerializeDataTable;
    REFLECT_FIELDS_END(EnemyActionSystem)
    std::map<int, EnemyAction::ActionData*> _enemyActionTable; // Action ID와 이름을 매핑하는 테이블

    ////////////////////////////////////////////////
    // Editor Only
    ////////////////////////////////////////////////

    Timeline::SequencerEditor& GetSequencerEditor();
    void                       ShowEditor();
    void                       LeftGuiFrame(ImVec2 size);
    void                       RightGuiFrame(ImVec2 size);
    bool                       ChangeActionID(int oldID, int newID);

    bool _isShowEditor = false;     // 에디터 모드 여부
    int  _selectedActionID = 0;     // 선택된 액션 ID
    EditorDragState _dragHandler;

private:
    SingletonComponent<EnemyActionSystem> _singletonComponent{this};

};

// 스킬 ID, 이름, 타입, 이벤트 트랙