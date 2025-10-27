#pragma once
#include "RevelationElement/RevelationElement.h"
#include <ExcelParser/ImGuiColumnSheetParser.h>
#include "Utility/SingletonHelper.h"

class TurnAction;
class RevelationSystem : public Component
{
    USING_PROPERTY(RevelationSystem)      
    using ActionDataType = std::unordered_map<std::string, std::string>;
    using ElementDataType = std::vector<std::string>;
public:
    RevelationSystem();
    ~RevelationSystem() override;

public:
    /// <summary>
    /// 특정 슬롯에 Element를 장착합니다.
    /// </summary>
    /// <param name="slot :">장착할 슬롯</param>
    /// <param name="element :">장비 정보</param>
    /// <returns>기존 장착된 장비. 없으면 nullptr</returns>
    std::shared_ptr<RevelationElement> EquipPlayerElement(int slot, const RevelationElement& element);

    /// <summary>
    /// 플레이어의 계시를 인벤토리에서 제거합니다.
    /// </summary>
    /// <param name="slot"></param>
    /// <returns></returns>
    std::shared_ptr<RevelationElement> RemovePlayerElement(int slot);

    /// <summary>
    /// 플레이어의 인벤토리 제일 뒤쪽에 계시를 추가합니다.
    /// </summary>
    /// <param name="element :">추가할 계시</param>
    /// <returns></returns>
    const std::shared_ptr<RevelationElement>& PushBackRevelation(const RevelationElement& element);

    /// <summary>
    /// 플레이어의 인벤토리에 랜덤한 소멸 계시를 추가합니다.
    /// <param name="count :">추가할 갯수</param>
    /// </summary>
    void EquipRandomExtinctionElement(size_t count = 1);

    /// <summary>
    /// 플레이어가 인벤토리 뒤쪽에 가지고 있는 모든 소멸 계시를 지웁니다.
    /// </summary>
    void RemoveAllExtinctionElements();
   
    /// <summary>
    /// 이번 라운드 활성화 계시를 랜덤으로 뽑습니다.
    /// </summary>
    void RollRoundElement();

    /// <summary>
    /// 이번 라운드에 활성화된 계시 항목을 반환합니다.
    /// </summary>
    /// <returns></returns>
    const MVVM::Model<std::vector<std::shared_ptr<RevelationElement>>>& GetRoundElementList()
    {
        return _roundElementList;
    }

    /// <summary>
    /// 플레이어가 사용중인 element 항목을 반환합니다.
    /// </summary>
    /// <returns></returns>
    const std::vector<std::shared_ptr<RevelationElement>>& GetPlayerElementList() { return _playerElementList; }

    /// <summary>
    /// 테이블의 모든 계시들을 반환합니다.
    /// </summary>
    /// <returns></returns>
    const std::vector<RevelationElement*>& GetRevelationTableElements() { return _elementTableOrderID; }
  
    /// <summary>
    /// 이번 턴 계시 발동 여부를 확인합니다.
    /// </summary>
    /// <param name="slot"></param>
    /// <returns></returns>
    bool IsCurrentTurnRevelationActive(size_t slot) 
    {
        if (slot < _currentTurnRevelationActiveFlag.size())
        {
            return _currentTurnRevelationActiveFlag[slot];
        }
        return false;
    }

public:     
    /// <summary>
    /// 새로운 Element를 테이블에 추가합니다.
    /// </summary>
    /// <param name="element"></param>
    bool InsertElement(const RevelationElement& element);

    /// <summary>
    /// Element를 테이블에서 제거합니다.
    /// </summary>
    /// <param name="elementName"></param>
    /// <returns></returns>
    bool EraseElement(std::string_view elementName);

    void ClearTable();

    /// <summary>
    /// Element를 테이블에서 찾아서 존재하면 반환합니다.
    /// </summary>
    /// <param name="elementName :">찾을 계시 이름</param>
    /// <returns>없으면 nullptr</returns>
    RevelationElement* FindElement(const std::string& elementName);

    /// <summary>
    /// ElementTable과 Action들을 Json으로 직렬화해 반환합니다.
    /// </summary>
    /// <returns></returns>
   std::string SaveElementTable() 
    { 
        ElementsToElementDatas();
        ActionsToActionDatas();
        std::string result = rfl::json::write(std::pair{rfl::json::write(ReflectFields->RevelationElementDatas),
                                                        rfl::json::write(ReflectFields->RevelationActionDatas)});
        return result; 
    }

    /// <summary>
    /// Json으로 직렬화한 Element Table과 Action들을 역직렬화 합니다.
    /// </summary>
    /// <param name="data :">json 형식의 문자열</param>
    /// <returns>결과</returns>
    bool LoadElementTable(std::string_view data)
    {
        auto result = rfl::json::read<std::pair<std::string, std::string>>(data.data());
        if (result)
        {
            auto& [elementData, actionData] = result.value();
            auto element = rfl::json::read<ElementDataType>(elementData.data());
            if (element)
            {
                ReflectFields->RevelationElementDatas = element.value();
                ElementDatasToElements();
            }
            auto action = rfl::json::read<ActionDataType>(actionData.data());
            if (action)
            {
                ReflectFields->RevelationActionDatas = action.value();
                ActionDatasToActions();
            }
        }
        return result;
    }

private:
    bool _tableEditorOpen = false;

    struct ImGuiEvent
    {
        std::function<void()>          RenameFunc;
        RevelationElement*             SelectElement     = nullptr;
        std::string                    DeleteTableBuffer = STR_NULL;
        bool                           OpenDeletePopup   = false;

        std::queue<RevelationElement*> DirtyRevelationElementQueue;
        bool                           ShowDirtyElementPopup = false;
    };
#ifdef _UMEDITOR
    ImGuiEvent _imguiEvent;
#endif
    void ImGuiDrawElementTableEditor();
    void ImGuiDrawExcelParser();
    bool ExcelToRevelationElement(RevelationElement& element, const std::string& key, const std::string& data);

public:
    REFLECT_PROPERTY(
        RevelationsPerRound)

    GETTER(int, RevelationsPerRound) { return ReflectFields->RevelationsPerRound; }
    SETTER(int, RevelationsPerRound) 
    { 
        int playerRevelationsCount = std::max(1, (int)_playerElementList.size());
        ReflectFields->RevelationsPerRound = std::clamp(value, 1, playerRevelationsCount);
    }
    // 라운드당 뽑는 계시 개수
    PROPERTY(RevelationsPerRound)

protected:
    REFLECT_FIELDS_BEGIN(Component)
    ActionDataType  RevelationActionDatas;
    ElementDataType RevelationElementDatas;
    int             RevelationsPerRound = 3;// 라운드당 계시
    ElementDataType PlayerElementDatas;
    REFLECT_FIELDS_END(RevelationSystem)

    void ActionsToActionDatas();
    void ActionDatasToActions();

    void ElementsToElementDatas();
    void ElementDatasToElements();

    void PlayerElementDatasToPlayerElements();
    void PlayerElementsToPlayerElementDatas();

    /// <summary>
    /// <para> 직렬화 직전 자동으로 호출되는 이벤트 함수입니다. </para>
    /// <para> 직접 override 해서 사용합니다.                 </para>
    /// </summary>
    void SerializedReflectEvent() override;

    /// <summary>
    /// <para> 역직렬화 이후 자동으로 호출되는 이벤트 함수 입니다.  </para>
    /// <para> 직접 override 해서 사용합니다.                     </para>
    /// </summary>
    void DeserializedReflectEvent() override;

    /// <summary>
    /// <para>  ImGuiDrawPropertys() 호출 이후 콜되는 이벤트 함수입니다. </para>
    /// </summary>
    void ImGuiDrawPropertysEvent() override;

private:
    std::map<std::string, RevelationElement> _elementsTable; // 계시 테이블
    std::vector<RevelationElement*>          _elementTableOrderID;
    ImVec2                                   _tableEditorCenterPos{};

private:
    SingletonComponent<RevelationSystem>            _singletonComponent{this};
    std::vector<std::shared_ptr<RevelationElement>> _playerElementList;                    // 플레이어가 사용중인 계시 (인벤토리)
    MVVM::Model<std::vector<std::shared_ptr<RevelationElement>>> _roundElementList;        // 이번 라운드에 효과가 발동된 계시 (뽑힌 계시)
    std::unordered_map<std::string, unsigned int>   _elementTotalAppearances;              // 계시가 뽑힌 횟수
    unsigned int                                    _totalRollCount = 0;                   //계시를 굴린 횟수
    std::vector<bool>                               _currentTurnRevelationActiveFlag;      //이번 턴 발동 여부

private:
    void ImGuiDrawPlayerElementEditor();
    void ImGuiDrawRoundElementList();
    void SortElementTableOrderID();
    void PushElementTableOrderID(RevelationElement& element);
    void EraseElementTableOrderID(RevelationElement& element);

protected:
    void Awake() override;
    void Reset() override;

};
